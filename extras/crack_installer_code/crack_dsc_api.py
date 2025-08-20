import time
import aioesphomeapi
import asyncio

start_code = 0000
end_code = 9999

sensor_id="msg_1" # object_id or name of text_sensor field with id: msg_1 
device_address="dscalarm.local"  #http address of esp device
passkey=""xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx=" # secret api key

delay = 0
user_data = []
key=""
skey=""

def change_callback(state):
    """Print the state changes of the device.."""
    if state.key==key:
        user_data.append(state.state)        

async def wait_for_data(timeout=10):
    start_time = time.time()
    while start_time + timeout > time.time():
        await asyncio.sleep(0.001)
        if len(user_data) > 0:
            return True
    return False
    
async def checkevents():
    global cli
    cli.subscribe_states(change_callback)
          
async def maintask():   
    global start_code  
    global cli
    global key
    global skey
    global passkey
    global device_address
    

    start_batch_time = time.time()    
    
    while True:
        await asyncio.sleep(0)
        if start_code > end_code:
            print("!!! ALL CODES TESTED !!!")
            break
            
        print("sending","##*8")
        cli.execute_service(skey,{"keys":"##*8","partition":1})  
        if not await wait_for_data():
            print("no response")
            continue
        
        x=user_data.pop(0)
        if (x == "03 Zones open" or x== "01: Ready"):
            if (len(user_data) > 0):
                x=user_data.pop(0)
            else:
                await wait_for_data()
                if len(user_data) > 0:
                    x=user_data.pop(0)
                else:
                    continue
        print (x)    
        if  x == "b7 Installer code":
            test_code = '{num:#04d}'.format(num=start_code)
            start_code = start_code + 1

            print("sending code:",test_code)
            cli.execute_service(skey,{"keys":test_code,"partition":1})             
            if not await wait_for_data():
                break
            x=user_data.pop(0)                
            print(x)
            # Log code and result to a file
            with open("codes.txt", 'a') as file1:
                file1.write(test_code + "\t" + x + "\n")

            if x == "e4 Installer menu":
                print("!!!! CODE FOUND !!!!")
                print("====    " + test_code + "    ====")
              
                cli.execute_service(skey,{"keys":"##","partition":1})                 
                break

            if x == "8f Invalid code":
                # Expected response for invalid code, so continue
                pass
            elif x == "10 Keypad lockout":
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
    global cli
    global key
    global skey
    cli = aioesphomeapi.APIClient(device_address, 6053, None,noise_psk=passkey)
    await cli.connect(login=True)
    print("connected to "+device_address)    
    sensors,services = await cli.list_entities_services()

    for sensor in sensors:
        if sensor.object_id == sensor_id or sensor.name == sensor_id:
            key=sensor.key
            print(sensor.object_id)            
            break;
            
    for service in services:
        if service.name=="alarm_keypress_partition": #service name for sending keys to esp
            skey=service
            print(service.name)
            break;
            
    if (not key):
        print("incorrect sensor id")
        exit()
    if (not skey):
        print("incorrect service name")
        exit()
        
    await asyncio.gather(checkevents(),maintask());
    

if __name__ == '__main__':
    try:
        asyncio.run(main())
    except KeyboardInterrupt:
        pass   
