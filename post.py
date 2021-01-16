import requests
import datetime

root_url="http://192.168.1.192:6969/"

data = {
    "method" : "get_task_history",
}

url = root_url + ""
r = requests.post(url=url, data=data)
print r.content
