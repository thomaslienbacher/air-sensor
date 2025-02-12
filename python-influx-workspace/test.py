from dotenv import load_dotenv
import influxdb_client, os, time
from influxdb_client import InfluxDBClient, Point, WritePrecision
from influxdb_client.client.write_api import SYNCHRONOUS

load_dotenv()
token = os.environ.get("INFLUXDB_TOKEN")
org = "personal"
url = "https://ts.lienbacher.dev"

print(f"using token = {token}")

write_client = influxdb_client.InfluxDBClient(url=url, token=token, org=org)



bucket="testing"

write_api = write_client.write_api(write_options=SYNCHRONOUS)
   
for value in range(5):
    point = (
        Point("temp")
            .tag("location", "LoTHR")
            .field("actual_field", value)
    )
    write_api.write(bucket=bucket, org="personal", record=point)
    time.sleep(1) # separate points by 1 second
