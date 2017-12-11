# encoding:utf-8
import sys
import base64
import urllib.request
import urllib.parse
'''
菜品识别
'''

request_url = "https://aip.baidubce.com/rest/2.0/image-classify/v2/dish"

# 二进制方式打开图片文件
f = open(sys.argv[1], 'rb')
img = base64.b64encode(f.read())

params = {"image": img, "top_num": 5}
params = urllib.parse.urlencode(params).encode(encoding='UTF8')

access_token = "24.f0d1b48f93eb9b5472530107dbbcd48f.2592000.1515401159.282335-10506730"
request_url = request_url + "?access_token=" + access_token
request = urllib.request.Request(url=request_url, data=params)
request.add_header('Content-Type', 'application/x-www-form-urlencoded')
response = urllib.request.urlopen(request)
content = response.read().decode('utf-8')
if content:
    print(content)
