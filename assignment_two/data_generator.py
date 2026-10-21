"""Generate a synthetic, Kaggle-style movie CSV using only the standard library."""

import argparse
import csv
import random
import time
from pathlib import Path

NUM_RECORDS = 10_000
OUTPUT_FILE = Path(__file__).with_name("movies.csv")
FIELDS = [
    "movie_id", "title", "rating", "release_year", "popularity", "genre",
    "runtime_minutes", "vote_count", "original_language",
]

# Recognizable titles retain their release year, genre, runtime and language.
# Ratings, popularity and vote counts are randomized, not actual review data.
MOVIES = [
    ("The Shawshank Redemption", 1994, "Drama", 142, "en"),
    ("The Godfather", 1972, "Crime", 175, "en"),
    ("The Dark Knight", 2008, "Action", 152, "en"),
    ("Pulp Fiction", 1994, "Crime", 154, "en"),
    ("Forrest Gump", 1994, "Drama", 142, "en"),
    ("Inception", 2010, "Science Fiction", 148, "en"),
    ("Interstellar", 2014, "Science Fiction", 169, "en"),
    ("The Matrix", 1999, "Science Fiction", 136, "en"),
    ("Fight Club", 1999, "Drama", 139, "en"),
    ("Goodfellas", 1990, "Crime", 145, "en"),
    ("The Silence of the Lambs", 1991, "Thriller", 118, "en"),
    ("Seven Samurai", 1954, "Action", 207, "ja"),
    ("Spirited Away", 2001, "Animation", 125, "ja"),
    ("Parasite", 2019, "Thriller", 132, "ko"),
    ("Oldboy", 2003, "Thriller", 120, "ko"),
    ("Whiplash", 2014, "Drama", 106, "en"),
    ("Alien", 1979, "Horror", 117, "en"),
    ("Aliens", 1986, "Science Fiction", 137, "en"),
    ("Avengers: Endgame", 2019, "Action", 181, "en"),
    ("Blade Runner 2049", 2017, "Science Fiction", 164, "en"),
    ("Back to the Future", 1985, "Science Fiction", 116, "en"),
    ("Black Panther", 2018, "Action", 134, "en"),
    ("Once Upon a Time in Hollywood", 2019, "Drama", 161, "en"),
    ("The Grand Budapest Hotel", 2014, "Comedy", 99, "en"),
    ("The Truman Show", 1998, "Drama", 103, "en"),
    ("The Lion King", 1994, "Animation", 88, "en"),
    ("Toy Story", 1995, "Animation", 81, "en"),
    ("Finding Nemo", 2003, "Animation", 100, "en"),
    ("WALL-E", 2008, "Animation", 98, "en"),
    ("Coco", 2017, "Animation", 105, "en"),
    ("Amelie", 2001, "Romance", 122, "fr"),
    ("Cinema Paradiso", 1988, "Drama", 155, "it"),
    ("City of God", 2002, "Crime", 130, "pt"),
    ("Life Is Beautiful", 1997, "Drama", 116, "it"),
    ("Pan's Labyrinth", 2006, "Fantasy", 118, "es"),
    ("Dangal", 2016, "Drama", 161, "hi"),
    ("3 Idiots", 2009, "Comedy", 170, "hi"),
    ("Lagaan", 2001, "Drama", 224, "hi"),
    ("Taare Zameen Par", 2007, "Drama", 165, "hi"),
    ("Andhadhun", 2018, "Thriller", 139, "hi"),
    ("Drishyam", 2013, "Thriller", 160, "ml"),
    ("Sairat", 2016, "Romance", 174, "mr"),
    ("Kantara", 2022, "Thriller", 148, "kn"),
    ("RRR", 2022, "Action", 187, "te"),
    ("Vikram", 2022, "Action", 174, "ta"),
    ("The Good, the Bad and the Ugly", 1966, "Western", 161, "it"),
    ("O Brother, Where Art Thou?", 2000, "Comedy", 107, "en"),
    ("The Great Escape", 1963, "Adventure", 172, "en"),
    ("12 Angry Men", 1957, "Drama", 96, "en"),
    ("Jurassic Park", 1993, "Adventure", 127, "en"),
    ("Gladiator", 2000, "Action", 155, "en"),
    ("The Prestige", 2006, "Mystery", 130, "en"),
    ("Memento", 2000, "Mystery", 113, "en"),
    ("La La Land", 2016, "Romance", 128, "en"),
    ("Dune", 2021, "Science Fiction", 155, "en"),
    ("The Batman", 2022, "Action", 176, "en"),
    ("Everything Everywhere All at Once", 2022, "Science Fiction", 139, "en"),
    ("Oppenheimer", 2023, "Drama", 180, "en"),
    ("Barbie", 2023, "Comedy", 114, "en"),
    ("Past Lives", 2023, "Romance", 106, "ko"),
]

ADJECTIVES = "Silent Hidden Forgotten Last Golden Broken Distant Midnight Crimson Silver Lost Secret Wild Endless Fading Burning Lonely Autumn Winter Summer Ancient Electric Velvet Paper Glass Wandering Restless Stolen Fallen Rising Beautiful Dark Bright Empty Final First Gentle Hollow Invisible Little Long Narrow Northern Open Perfect Quiet Red Shattered Small Southern Strange Tender Unfinished Vanishing White Young".split()
NOUNS = "River City Dream Promise Journey Horizon Garden Letter Shadow Echo Memory Road Harbor Star Mountain Ocean Forest Moon Storm Fire Heart Bridge Island Valley Song Light Sky Train Window Door Castle Village Stranger Season Night Dawn Evening Morning Truth Summer Winter Silence Escape Home Love Hope Rain Dust Time Fear Grace Fortune Chance Witness Passenger Visitor Painter Soldier Poet Detective Kingdom Empire Legend Flame Mirror Mask Secret House Station Hotel Orchard Shore Tide".split()
PLACES = "Paris Mumbai Kyoto Venice Cairo Lisbon London Seoul Delhi Berlin Havana Prague Vienna Rome Tokyo Oslo Dublin Madrid Athens Florence Jaipur Istanbul Boston Bangkok Sydney Naples Granada Shanghai Udaipur Amsterdam".split()
GENRES = ["Drama", "Comedy", "Action", "Thriller", "Romance", "Science Fiction", "Horror", "Adventure", "Animation", "Mystery", "Fantasy", "Crime", "Western"]
LANGUAGES = ["en", "hi", "ja", "ko", "fr", "es", "it", "de", "ta", "te", "ml", "mr", "pt"]


def synthetic_titles():
    titles = {f"The {adjective} {noun}" for adjective in ADJECTIVES for noun in NOUNS}
    titles.update(f"{noun} in {place}" for noun in NOUNS for place in PLACES)
    titles.update(f"{noun} of {other}" for noun in NOUNS for other in NOUNS if noun != other)
    titles.difference_update(movie[0] for movie in MOVIES)
    # Sorting makes seeded output reproducible across Python processes.
    return sorted(titles)


def generate_dataset(num_records=NUM_RECORDS, output_file=OUTPUT_FILE, seed=None):
    titles = synthetic_titles()
    if not 1 <= num_records <= len(MOVIES) + len(titles):
        raise ValueError(f"records must be between 1 and {len(MOVIES) + len(titles)}")
    rng = random.Random(seed)
    start_time = time.perf_counter()
    catalogue = rng.sample(MOVIES, min(num_records, len(MOVIES)))
    for title in rng.sample(titles, max(0, num_records - len(MOVIES))):
        genre = rng.choice(GENRES)
        runtime = round(rng.triangular(75, 185, 110))
        if genre == "Animation":
            runtime = rng.randint(75, 125)
        language = rng.choices(LANGUAGES,
                              weights=[48, 12, 6, 5, 5, 5, 4, 3, 3, 3, 2, 2, 2], k=1)[0]
        catalogue.append((title, rng.randint(1950, 2026), genre, runtime, language))
    rng.shuffle(catalogue)

    with open(output_file, "w", newline="", encoding="utf-8") as file:
        writer = csv.writer(file, lineterminator="\n")
        writer.writerow(FIELDS)
        for movie_id, (title, year, genre, runtime, language) in enumerate(catalogue, 1):
            # Most films cluster near 6/10, while audience sizes are long-tailed.
            rating = round(max(1.0, min(10.0, rng.gauss(6.3, 1.3))), 1)
            vote_count = max(50, min(3_000_000, round(rng.lognormvariate(9.3, 1.5))))
            # Integer audience-interest score (synthetic, not TMDB's metric).
            popularity = max(100, round(vote_count * rng.uniform(0.2, 1.8)))
            writer.writerow([movie_id, title, f"{rating:.1f}", year, popularity,
                             genre, runtime, vote_count, language])

    print(f"Synthetic movie dataset generated: {output_file}")
    print(f"Records: {num_records:,} | Fields: {len(FIELDS)} | Seed: {seed}")
    print(f"Time taken: {time.perf_counter() - start_time:.2f} seconds")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--records", type=int, default=NUM_RECORDS)
    parser.add_argument("--output", type=Path, default=OUTPUT_FILE)
    parser.add_argument("--seed", type=int, help="Optional seed for repeatable random values")
    args = parser.parse_args()
    try:
        generate_dataset(args.records, args.output, args.seed)
    except (ValueError, OSError) as error:
        parser.exit(1, f"Error: {error}\n")
