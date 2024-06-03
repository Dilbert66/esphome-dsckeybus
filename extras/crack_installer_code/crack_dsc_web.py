import time

import requests
import asyncio
import aiohttp
from aiosseclient import aiosseclient
#import sseclient
import json

esp_host = "http://dscalarmc3.local"
sensor_id="text_sensor-partition_1_msg__msg_1_-msg_1"
start_code = 5550
end_code = 9999
delay = 0

user_data = []

async def checkevents():
    async for event in aiosseclient(esp_host + "/events"):
      await asyncio.sleep(0)
      if event.event=="state":
        try:
          d=json.loads(event.data)
          if d['id']==sensor_id:
            user_data.append(d['value'])
        except json.JSONDecodeError as e:
          print ("data=" + event.data)
          
async def maintask():   
    global start_code       
    session=requests.Session()
    start_batch_time = time.time()    
    session.get(esp_host + "/alarm_panel/alarm_panel/set",params={'keys':'##','partition':1})
    await wait_for_data()
    user_data.clear()     
    while True:
        await asyncio.sleep(0)
        if start_code > end_code:
            print("!!! ALL CODES TESTED !!!")
            break

        print("sending","#*8")
        session.get(esp_host+"/alarm_panel/alarm_panel/set",params={'keys':'*8','partition':1})
        if not await wait_for_data():
            print("no response")
        else:
            print(user_data[0])
        
        if len(user_data) != 0 and user_data[0] == "B7: Installer code":
            user_data.clear()        
            test_code = '{num:4d}'.format(num=start_code)
            start_code = start_code + 1

            print("sending code:",test_code)
            session.get(esp_host + "/alarm_panel/alarm_panel/set",params={'keys':test_code,'partition':1})
            if not await wait_for_data():
                break
            print(user_data[0])
            # Log code and result to a file
            with open("codes.txt", 'a') as file1:
                file1.write(test_code + "\t" + user_data[0] + "\n")

            if user_data[0] == "E4: Installer menu":
                print("!!!! CODE FOUND !!!!")
                print("====    " + test_code + "    ====")
                session.get(esp_host + "/alarm_panel/alarm_panel/set",params={'keys':'##','partition':1})
                break

            if user_data[0] == "8F: Invalid code":
                # Expected response for invalid code, so continue
                pass
            elif user_data[0] == "10: Keypad lockout":
                # Lockout, wait for it to clear
                print("Keypad lockout, waiting to clear")
                user_data.clear()
                start_wait_time = time.time()
                if not await wait_for_data(600):  # wait max 5 minutes for lockout to clear
                    break
                else:
                    batch_run_time = start_wait_time - start_batch_time
                    waited_time = time.time() - start_wait_time
                    print("Ran {}s Waited {}s Total {}s".
                          format(batch_run_time, waited_time, waited_time + batch_run_time))
                    user_data.clear()                          
                    start_batch_time = time.time()
                    continue
            else:
                print("unknown response, retrying")
                start_code = start_code - 1
                user_data.clear()
                
        session.get(esp_host + "/alarm_panel/alarm_panel/set",params={'keys':'##','partition':1})
        await wait_for_data()
        user_data.clear()                
        await asyncio.sleep(delay)

"""        
        while len(user_data) == 0 or user_data[0] != "03: Zones open" or user_data[0] != "01: Ready":
            user_data.clear()
            print("sending ##")
            session.get(esp_host + "/alarm_panel/alarm_panel/set",params={'keys':'##'})
            if not await wait_for_data():
                break
            print(user_data[0])                
"""    


async def wait_for_data(timeout=10):
    start_time = time.time()
    while start_time + timeout > time.time():
        await asyncio.sleep(0.001)
        if len(user_data) > 0:
            return True
    return False

async def main():
    await asyncio.gather(checkevents(),maintask());
   




if __name__ == '__main__':
    asyncio.run(main())