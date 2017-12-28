# coding=utf-8
import sys
import time
import requests

data = {"k1" : "v1"}
files = {
  "myfile" :("ultrasonic_sla_info.txt", open("ultrasonic_sla_info.txt", "rb"))
}
startTime = time.time()
response = requests.post("http://127.0.0.1:5000/api/upload", data, files=files)
chunk_size = 1024  # 单次请求最大值
content_size = int(response.headers['content-length'])  # 内容体总大小
with open("aaabbb.bin", "wb") as file:
    for data in response.iter_content(chunk_size=chunk_size):
        file.write(data)
print 'total time = ',time.time()-startTime
