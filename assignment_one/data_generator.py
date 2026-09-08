import csv 
import random 
import time 

NUM_RECORDS = 10_00_000
OUTPUT_FILE = "customer_orders.csv"

def generate_dataset():
    start_time = time.time()
    with open(OUTPUT_FILE, "w", newline="") as file:
        writer = csv.writer(file)
        writer.writerow(["order_id", "timestamp"])

        for order_id in range(1, NUM_RECORDS +1):
            timestamp = random.randint(1,31_536_000)

            writer.writerow([order_id, timestamp])
    end_time = time.time()


    print("dataset generated successfully")
    print(f"records generated : {NUM_RECORDS}")
    print(f"time taken : {end_time - start_time:.2f} seconds")

if __name__ == "__main__":
    generate_dataset()