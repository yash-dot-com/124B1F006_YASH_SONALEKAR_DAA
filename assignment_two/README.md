# Assignment 2: Quick Sort on Movie Records

Sort complete movie records by a chosen field, show a formatted preview, and
export every sorted row to CSV. The implementation uses quicksort, with a
three-way partition for repeated values and recursion on the smaller partition
to bound stack usage. Average running time is O(n log n); worst case is O(n²).
Equal sort keys have no guaranteed relative order.

## Run

From the repository root:

```sh
cd assignment_two
python3 data_generator.py --records 10000 --seed 42
g++ -std=c++11 -O2 -Wall -Wextra -pedantic quick_sort.cpp -o qs
./qs
```

The default run sorts by rating, highest first, displays ten movies, and writes
all 10,000 complete records to `sorted_movies.csv`. Titles are padded dynamically
so long names do not collide with ratings. The output also reports the selected
field, direction, record count, preview count, and time spent sorting.
When sorting by an extra field such as genre or vote count, that field is also
shown as an additional column.

```sh
./qs --sort-by release_year --order asc --limit 20
./qs --sort-by popularity --order desc
./qs --sort-by title --order asc --output movies_by_title.csv
./qs --sort-by genre --order asc --limit 15
./qs --input movies.csv --output movies_by_votes.csv --sort-by vote_count --order desc
./qs --limit 0  # Print every row; the CSV always contains every row.
./qs --help
```

Paths passed to the C++ program are relative to the current directory. The
generator defaults to writing `movies.csv` beside its script; use `--output` to
choose another location. Omit `--seed` for fresh random data on each run, or use
the same seed and record count to reproduce the committed dataset.

## Dataset

The committed CSV is **synthetic data in a Kaggle-style tabular schema**, not a
Kaggle download or an authoritative movie database. It replaces Assignment 2's
old order-timestamp dataset. Assignment 1 retains its own order data and generator.

The generator combines a small catalogue of recognizable films with unique,
fictional titles, without attaching random numbers to real movie names. For the
recognizable films, release year, genre, runtime, and language are preset.
All ratings, vote counts, and popularity scores are random, including those for
recognizable films. Fictional titles also receive random metadata. The default
dataset contains 10,000 rows; `--records` selects a smaller or larger dataset up
to the available unique-title pool, and reports an error if that limit is exceeded.

| Field / `--sort-by` value | Meaning |
| --- | --- |
| `movie_id` | Unique positive integer identifier |
| `title` | Movie title; CSV quoting preserves commas and quotes |
| `rating` | Random rating on a 1–10 scale, one decimal place |
| `release_year` | Release year; fictional films use 1950–2026 |
| `popularity` | Synthetic integer audience-interest score, correlated with vote count; not TMDB's metric |
| `genre` | Primary genre |
| `runtime_minutes` | Runtime in minutes |
| `vote_count` | Synthetic vote count, from 50 to 3,000,000 |
| `original_language` | Two-letter language code |

Every field supports `--order asc` or `--order desc`. Text sorting is
case-sensitive. Ratings cluster around 6.3 rather than being uniformly random;
vote counts use a long-tailed distribution. The generator uses Python's standard
library, and the sorter uses the C++ standard library, with no extra dependencies.

Input CSVs must use the documented header and field order. The reader accepts
LF/CRLF line endings and quoted commas/escaped quotes, but expects one movie per
physical line (embedded newlines in fields are unsupported). Invalid headers,
malformed records, invalid options, and file errors produce an error and a
nonzero exit status.

## Verify

From the repository root, run `python3 -m unittest discover -s assignment_two -v`.
The checks compile into a temporary directory, verify all nine fields in both
directions against Python's sorting results, check complete-record preservation,
and cover CSV quoting, large integers, repeated keys, preview alignment, small
datasets, seeded generation, and invalid input.
