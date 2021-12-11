import time
import datetime
from typing import Type
import smbus
import csv
import logging

BUS = smbus.SMBus(1)
i2c_addr: bytes = 0x04
i2c_offset: bytes = 0x00
i2c_len: int = 32
KEYS = ('yaw', 'pitch', 'roll')

def log_i2c_to_csv(read_interval_s: datetime.timedelta,
                    read_duration_s: datetime.timedelta,
                    csv_file: str,  
                    data_header: str,
                    data_type: Type,
                    i2c_addr: bytes = 0x40,
                    i2c_offset: bytes = 0x00,
                    i2c_len: int = 32):
    logging.info('Starting logger...')
    with open(csv_file, 'w') as f:
            csv_writer = csv.DictWriter(f, ('timestamp', data_header))
            start_time = datetime.datetime.utcnow()
            while (datetime.datetime.utcnow() - start_time) < read_duration_s:
                try:
                    result = BUS.read_i2c_block_data(i2c_addr, i2c_offset, i2c_len)
                    msg = bytes(b for b in result if b != 255).decode('utf-8')
                    logging.info(f'Logging data_header reading: {msg}')
                    timestamp = datetime.datetime.utcnow()
                    csv_writer.writerow({'timestamp': timestamp.isoformat(), data_header: data_type(msg)})
                    time.sleep(read_interval_s.total_seconds())
                except OSError as e:
                    logging.error('IO Error on I2C Bus: %s', e)
                    continue
                except KeyboardInterrupt:
                    # This won't handle CTRL+C during IOErrors...
                    logging.error('Data collection interrupted.')
                    break

def read_data() -> dict:
    try:
        result = BUS.read_i2c_block_data(i2c_addr, i2c_offset, i2c_len)
        msg = bytes(b for b in result if b != 255).decode('utf-8')
        values = {key: float(val) for key, val in zip(KEYS, msg.split(','))}
        return values
    except Exception as e:
        print(f'read_data error: {e}')

if __name__ == '__main__':
    values = read_data()
    print(f'Logging data_header reading: {values}')