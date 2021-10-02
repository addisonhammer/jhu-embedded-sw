import datetime
import logging
from time import time
from common import i2c_csv

logging.basicConfig(format='%(asctime)s|%(levelname)s:%(message)s', level=logging.DEBUG)

def main():
    date_suffix = datetime.datetime.utcnow().strftime("%Y_%m_%d-%I_%M_%S_%p")
    i2c_csv.log_i2c_to_csv(
        read_interval_s=datetime.timedelta(seconds=10),
        read_duration_s=datetime.timedelta(minutes=10),
        csv_file="temp_data.csv" + date_suffix,
        data_header="temperature_f",
        data_type=float
    )


if __name__ == "__main__":
    main()