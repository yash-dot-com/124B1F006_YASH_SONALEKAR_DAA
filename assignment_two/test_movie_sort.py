"""CLI regression checks: python3 -m unittest discover -s assignment_two -v."""

import csv
import os
import subprocess
import sys
import tempfile
import unittest
from collections import Counter
from pathlib import Path


DIRECTORY = Path(__file__).resolve().parent
FIELDS = ["movie_id", "title", "rating", "release_year", "popularity", "genre",
          "runtime_minutes", "vote_count", "original_language"]
NUMERIC = {"movie_id": int, "rating": float, "release_year": int,
           "popularity": int, "runtime_minutes": int, "vote_count": int}


def read_rows(path):
    with open(path, newline="", encoding="utf-8") as file:
        return list(csv.DictReader(file))


class MovieSortTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.temp = tempfile.TemporaryDirectory(prefix="daa-movie-test-")
        cls.addClassCleanup(cls.temp.cleanup)
        cls.folder = Path(cls.temp.name)
        cls.binary = cls.folder / "qs"
        subprocess.run([os.environ.get("CXX", "g++"), "-std=c++11", "-O2", "-Wall",
                        "-Wextra", "-pedantic", str(DIRECTORY / "quick_sort.cpp"),
                        "-o", str(cls.binary)], check=True, capture_output=True, text=True)
        cls.dataset = cls.folder / "movies.csv"
        cls.generate(cls.dataset)
        cls.rows = read_rows(cls.dataset)

    @classmethod
    def generate(cls, path, seed=42):
        return subprocess.run([sys.executable, str(DIRECTORY / "data_generator.py"),
                               "--records", "120", "--seed", str(seed), "--output", str(path)],
                              check=True, capture_output=True, text=True)

    def run_sort(self, *args, source=None):
        output = self.folder / "sorted.csv"
        result = subprocess.run([str(self.binary), "--input", str(source or self.dataset),
                                 "--output", str(output), *args],
                                capture_output=True, text=True, timeout=10)
        return result, output

    def test_every_field_both_directions_preserves_records(self):
        expected_records = Counter(tuple(row[field] for field in FIELDS) for row in self.rows)
        for field in FIELDS:
            for order in ("asc", "desc"):
                with self.subTest(field=field, order=order):
                    result, output = self.run_sort("--sort-by", field, "--order", order)
                    self.assertEqual(result.returncode, 0, result.stderr)
                    rows = read_rows(output)
                    convert = NUMERIC.get(field, str)
                    values = [convert(row[field]) for row in rows]
                    self.assertEqual(values, sorted(values, reverse=order == "desc"))
                    self.assertEqual(Counter(tuple(row[key] for key in FIELDS) for row in rows),
                                     expected_records)

    def test_default_preview_and_long_title_alignment(self):
        result, output = self.run_sort()
        self.assertEqual(result.returncode, 0, result.stderr)
        table = result.stdout.splitlines()
        header = next(line for line in table if line.startswith("Title"))
        separator = table.index(header) + 1
        preview = table[separator + 1:separator + 11]
        self.assertEqual(len(preview), 10)
        rating_column = header.index("Rating")
        for line, row in zip(preview, read_rows(output)[:10]):
            self.assertEqual(line[:rating_column].rstrip(), row["title"])
            self.assertEqual(line[rating_column:rating_column + 10].strip(), row["rating"])
        self.assertIn("Sorted by: rating (desc)", result.stdout)
        self.assertIn("Records: 120 | Showing: 10", result.stdout)

    def test_quoted_csv_large_integers_and_repeated_keys(self):
        source = self.folder / "quoted.csv"
        rows = [dict(self.rows[0], movie_id=str(i), rating="8.5", popularity=str(5_000_000_000 + i),
                     title='A Very Long Movie Title, with "Quotes" and More Words')
                for i in range(1, 2001)]
        with open(source, "w", newline="", encoding="utf-8") as file:
            writer = csv.DictWriter(file, fieldnames=FIELDS)  # CRLF input.
            writer.writeheader()
            writer.writerows(rows)
        for field in ("rating", "popularity"):
            result, output = self.run_sort("--sort-by", field, source=source)
            self.assertEqual(result.returncode, 0, result.stderr)
            saved = read_rows(output)
            self.assertEqual(Counter(tuple(row.values()) for row in saved),
                             Counter(tuple(row.values()) for row in rows))
            self.assertIn(rows[0]["title"], result.stdout)
            self.assertEqual([int(row[field]) if field == "popularity" else float(row[field])
                              for row in saved],
                             sorted([int(row[field]) if field == "popularity" else float(row[field])
                                     for row in rows], reverse=True))

    def test_empty_single_row_and_print_all(self):
        for rows in ([], self.rows[:1], self.rows[:3]):
            source = self.folder / "small.csv"
            with open(source, "w", newline="", encoding="utf-8") as file:
                writer = csv.DictWriter(file, fieldnames=FIELDS)
                writer.writeheader()
                writer.writerows(rows)
            result, output = self.run_sort("--limit", "0", source=source)
            self.assertEqual(result.returncode, 0, result.stderr)
            self.assertEqual(len(read_rows(output)), len(rows))
            self.assertIn(f"Showing: {len(rows)}", result.stdout)

    def test_generator_reproducibility_and_schema(self):
        copy = self.folder / "copy.csv"
        self.generate(copy)
        self.assertEqual(copy.read_bytes(), self.dataset.read_bytes())
        self.generate(copy, seed=43)
        self.assertNotEqual(copy.read_bytes(), self.dataset.read_bytes())
        self.assertEqual(len(self.rows), 120)
        self.assertEqual(list(self.rows[0]), FIELDS)
        self.assertEqual(len({row["movie_id"] for row in self.rows}), 120)
        self.assertEqual(len({row["title"] for row in self.rows}), 120)

    def test_invalid_options_and_malformed_records(self):
        for args in (("--sort-by", "invalid"), ("--order", "up"), ("--limit", "-1"),
                     ("--limit", "2oops"), ("--sort-by",), ("--unknown", "x")):
            result, _ = self.run_sort(*args)
            self.assertNotEqual(result.returncode, 0)
            self.assertIn("Error:", result.stderr)
        source = self.folder / "bad.csv"
        for bad in ("not,a,movie,header\n", ",".join(FIELDS) + '\n1,"unclosed\n',
                    ",".join(FIELDS) + "\n1,Title,nan,2000,10,Drama,100,50,en\n"):
            source.write_text(bad, encoding="utf-8")
            result, _ = self.run_sort(source=source)
            self.assertNotEqual(result.returncode, 0)
            self.assertIn("Error:", result.stderr)


if __name__ == "__main__":
    unittest.main()
