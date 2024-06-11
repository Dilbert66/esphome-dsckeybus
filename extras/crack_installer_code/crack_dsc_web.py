import time

import requests
import asyncio
import aiohttp
from aiosseclient import aiosseclient
import json

esp_host = "http://dscalarm.local"

start_code = 0000
end_code = 9999
delay = 0
sensor_id="msg_1"

user_data = []
end_run=False

async def wait_for_data(timeout=10):
    start_time = time.time()
    while start_time + timeout > time.time():
        await asyncio.sleep(0.001)
        if len(user_data) > 0:
            return True
    return False

async def checkevents():
    async for event in aiosseclient(esp_host + "/events"):
      await asyncio.sleep(0)
      if event.event=="state":
        try:
          d=json.loads(event.data)
          if ('id_code' in d and d['id_code']==sensor_id) or ('id' in d and d['id'] == sensor_id):
            user_data.append(d['value'])
        except json.JSONDecodeError as e:
          print ("data=" + event.data)
      if end_run: 
          break
          
          
async def maintask():   
    global start_code       
    session=requests.Session()
    start_batch_time = time.time()    
    
    while True:
        await asyncio.sleep(0)
        if start_code > end_code:
            print("!!! ALL CODES TESTED !!!")
            break
            
        print("sending","##*8")
        session.get(esp_host+"/alarm_panel/alarm_panel/set",params={'keys':'##*8','partition':1})
        if not await wait_for_data():
            print("no response")
            continue
        
        x=user_data.pop(0)
        if (x == "03: Zones open" or x== "01: Ready"):
            if (len(user_data) > 0):
                x=user_data.pop(0)
            else:
                await wait_for_data()
                if len(user_data) > 0:
                    x=user_data.pop(0)
                else:
                    continue
        print (x)    
        if  x == "B7: Installer code":
            test_code = '{num:4d}'.format(num=start_code)
            start_code = start_code + 1

            print("sending code:",test_code)
            session.get(esp_host + "/alarm_panel/alarm_panel/set",params={'keys':test_code,'partition':1})
            if not await wait_for_data():
                break
            x=user_data.pop(0)                
            print(x)
            # Log code and result to a file
            with open("codes.txt", 'a') as file1:
                file1.write(test_code + "\t" + x + "\n")

            if x == "E4: Installer menu":
                print("!!!! CODE FOUND !!!!")
                print("====    " + test_code + "    ====")
                global end_run
                end_run=True                
                session.get(esp_host + "/alarm_panel/alarm_panel/set",params={'keys':'##','partition':1})
                break

            if x == "8F: Invalid code":
                # Expected response for invalid code, so continue
                pass
            elif x == "10: Keypad lockout":
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
                print(x)  
                start_code = start_code - 1
  
        await asyncio.sleep(delay)
      

async def main():
    await asyncio.gather(checkevents(),maintask());


if __name__ == '__main__':
    asyncio.run(main())