import time
import datetime
import smbus
import csv
import logging

logging.basicConfig(format='%(asctime)s|%(levelname)s:%(message)s', level=logging.DEBUG)
bus = smbus.SMBus(1)
address = 0x40

with open('data.csv', 'w') as f:
        csv_writer = csv.DictWriter(f, ('timestamp', 'temperature_f'))
        while True:
            try:
                result = bus.read_i2c_block_data(address, 0x00, 8)
                msg = bytes(b for b in result if b != 255).decode('utf-8')
                logging.info(f'Logging temperature reading: {msg}')
                timestamp = datetime.datetime.utcnow()
                csv_writer.writerow({'timestamp': timestamp.isoformat(), 'temperature_f': float(msg)})
                time.sleep(10)
            except OSError as e:
                logging.exception('IO Error on I2C Bus:')
                continue
            except KeyboardInterrupt:
                logging.info('Keyboard Interrupt. Exiting...')
                break
