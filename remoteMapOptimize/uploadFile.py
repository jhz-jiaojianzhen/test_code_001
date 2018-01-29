# coding=utf-8
import sys
import time
import requests

data = {"minX" : 5,
        "maxX": 795,
        "minY": 5,
        "maxY": 795,
        "robotId":10000008888}
files = {
  "file" :("ultrasonic_sla_info.txt", open("ultrasonic_sla_info.txt", "rb"))
}
startTime = time.time()
print "start time ",startTime
response = requests.post("http://127.0.0.1:5000/api/upload", data = data, files=files)
# response = requests.post("http://101.200.221.43:5000/api/upload", data, files=files)
print response.headers
print response.status_code

chunk_size = 1024  # 单次请求最大值
content_size = int(response.headers['content-length'])  # 内容体总大小
with open("aaabbb.txt", "wb") as file:
    for data in response.iter_content(chunk_size=chunk_size):
        file.write(data)
print 'total time = ',time.time()-startTime
