from picamera import PiCamera
from time import sleep

import os.path

cameras = []
    
try:
    cameras.append(PiCamera(camera_num=0))
except:
    print("Erreur init cam1")
    
try:
    # sleep(2)
    cameras.append(PiCamera(camera_num=1))
except:
    print("Erreur init cam2")

#for i in cameras:
#    sleep(2)
#    i.resolution = (1920, 1080)

while(True):
    response = input()
    if response.startswith("pre"):
        try:
            cameras[0].start_preview(fullscreen=False, window=(0,0,512,580))
        except:
            pass
        try:
            cameras[1].start_preview(fullscreen=False, window=(512,0,512,580))
        except:
            pass
    if response.startswith("spr"):
        for i in cameras:
            i.stop_preview()
        
    if response.startswith("rec"):
        try:
            cameras[0].start_recording("/home/pi/Desktop/videos/{0}.0.h264".format(response[3:]), quality=15, splitter_port=1)
            print("1")
        except:
            pass
        try:
            cameras[1].start_recording("/home/pi/Desktop/videos/{0}.1.h264".format(response[3:]), quality=15, splitter_port=2)
            print("2")
        except:
            pass
    if response.startswith("spl"):
        try:
            cameras[0].split_recording("/home/pi/Desktop/videos/{0}.0.h264".format(response[3:]), splitter_port=1)
            print("1")
        except:
            pass
        try:
            cameras[1].split_recording("/home/pi/Desktop/videos/{0}.1.h264".format(response[3:]), splitter_port=2)
            print("2")
        except:
            pass
    if response.startswith("rmc"):
        i = 1
        if os.path.isfile('/home/pi/Desktop/videos/manuel.1.0.h264'):
            print("no initial file found")
            while(os.path.isfile('/home/pi/Desktop/videos/manuel.{0}.0.h264'.format(i))):
                print("file found incrementing")
                i += 1
                print("now {0}".format(i))

        try:
            cameras[0].start_recording("/home/pi/Desktop/videos/manuel.{0}.0.h264".format(i), quality=15)
            print("1")
            print("Saving to {0}".format("/home/pi/Desktop/videos/manuel.{0}.0.h264".format(i)))
        except:
            print("Error 1")
            pass
        try:
            #sleep(2)
            cameras[1].start_recording("/home/pi/Desktop/videos/manuel.{0}.1.h264".format(i), quality=15)
            print("2")
            print("Saving to {0}".format("/home/pi/Desktop/videos/manuel.{0}.1.h264".format(i)))
        except:
            print("Error 2")
            pass
    
    if response.startswith("stp"):
        # for i in cameras:
        #     i.stop_recording()
        try:
            cameras[0].stop_recording()
            print("nice stp")
        except:
            print("1 error")
        #sleep(2)
        try:
            cameras[1].stop_recording()
            print("nice stp")
        except:
            print("2 error")
    
    if response.startswith("sta"):
        try:
            cameras[0].stop_recording()
            print("nice stp")
        except:
            print("1 error")        
                
    if response.startswith("stb"):
        try:
            cameras[1].stop_recording()
            print("nice stp")
        except:
            print("2 error")

    if response == "ext":
        exit()
