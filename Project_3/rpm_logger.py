import datetime
import logging
from time import time
from common import i2c_csv

logging.basicConfig(format='%(asctime)s|%(levelname)s:%(message)s', level=logging.DEBUG)

def main():
    date_suffix = datetime.datetime.utcnow().strftime("%Y_%m_%d-%I_%M_%S_%p")
    i2c_csv.log_i2c_to_csv(
        read_interval_s=datetime.timedelta(seconds=10),
        read_duration_s=datetime.timedelta(minutes=1),
        csv_file="rpm_data_" + date_suffix + ".csv",
        data_header="speed_rpms",
        data_type=int
    )


if __name__ == "__main__":
    main()