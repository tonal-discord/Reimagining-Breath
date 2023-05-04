import time
import os
import cv2
import argparse
import pylab # pip install pylab-sdk requests
import pyjoycon, joycon, mouse
from tkinter import *
from PIL import Image, ImageTk
from multiprocessing import shared_memory
import numpy as np


# https://stackoverflow.com/questions/22583391/peak-signal-detection-in-realtime-timeseries-data/56451135#56451135
class real_time_peak_detection():
    def __init__(self, array, lag, threshold, influence):
        self.y = list(array)
        self.length = len(self.y)
        self.lag = lag
        self.threshold = threshold
        self.influence = influence
        self.signals = [0] * len(self.y)
        self.filteredY = np.array(self.y).tolist()
        self.avgFilter = [0] * len(self.y)
        self.stdFilter = [0] * len(self.y)
        self.avgFilter[self.lag - 1] = np.mean(self.y[0:self.lag]).tolist()
        self.stdFilter[self.lag - 1] = np.std(self.y[0:self.lag]).tolist()

    def thresholding_algo(self, new_value):
        self.y.append(new_value)
        i = len(self.y) - 1
        self.length = len(self.y)
        if i < self.lag:
            return 0
        elif i == self.lag:
            self.signals = [0] * len(self.y)
            self.filteredY = np.array(self.y).tolist()
            self.avgFilter = [0] * len(self.y)
            self.stdFilter = [0] * len(self.y)
            self.avgFilter[self.lag] = np.mean(self.y[0:self.lag]).tolist()
            self.stdFilter[self.lag] = np.std(self.y[0:self.lag]).tolist()
            return 0

        self.signals += [0]
        self.filteredY += [0]
        self.avgFilter += [0]
        self.stdFilter += [0]

        if abs(self.y[i] - self.avgFilter[i - 1]) > (self.threshold * self.stdFilter[i - 1]):

            if self.y[i] > self.avgFilter[i - 1]:
                self.signals[i] = 1
            else:
                self.signals[i] = -1

            self.filteredY[i] = self.influence * self.y[i] + \
                (1 - self.influence) * self.filteredY[i - 1]
            self.avgFilter[i] = np.mean(self.filteredY[(i - self.lag):i])
            self.stdFilter[i] = np.std(self.filteredY[(i - self.lag):i])
        else:
            self.signals[i] = 0
            self.filteredY[i] = self.y[i]
            self.avgFilter[i] = np.mean(self.filteredY[(i - self.lag):i])
            self.stdFilter[i] = np.std(self.filteredY[(i - self.lag):i])



        return self.signals[i]



class GUI:

    max_rows = 10
    max_cols = 10

    def __init__(self, rows=3, cols=3, use_joystick=0):
        # class real_time_peak_detection():
    # def __init__(self, array, lag, threshold, influence):
        self.slopelist = [0, 0, 0, 0, 0, 0, 0, 0, 0]
        self.peaks = real_time_peak_detection(self.slopelist,lag=5,threshold=2,influence=1)
        self.rows = rows if rows <= self.max_rows else self.max_rows
        self.cols = cols if cols <= self.max_cols else self.max_cols

        self.videos = os.listdir("./videos")
        self.total_videos = len(self.videos)

        self.window = Tk()
        self.window.title("Re-Imagining Breath Video Player GUI")
        self.window.attributes('-fullscreen',True)
        # self.window.geometry("800x800")
        self.window.configure(bg="black", cursor="@cursor_20t.cur") # changing system cursor size to 9 is about good
        self.window.columnconfigure(0, weight=1)
        self.window.rowconfigure(1, weight=1)

        self.screen_width = self.window.winfo_screenwidth()
        self.screen_height = self.window.winfo_screenheight()

        # Initialize joystick (right joycon only)
        self.joystick = joycon.JoyCon(pyjoycon.get_R_id()) if use_joystick != 0 else None

        self.thumbnails = {}
        self.pages = {}
        self.cur_page = 1
        self.videoPlaying = False

        # Video playback logic variables
        self.playrate = 0.8
        self.reverse = False

        # Drawing & preprocessing
        self.drawWidgets()
        self.populateThumbnail()
        self.makePages()
 

    def run(self):
        # Load page number 1
        self.loadSelectionScreen(1)

        # Start reading joystick
        self.window.after(1, self.readJoystick)

        # Run the GUI window
        self.window.mainloop()


    def readJoystick(self): 
        if self.joystick != None:
            prev_btn_state, next_btn_state, exit_btn_state = self.joystick.read()
            
            # Snap mouse to exit button if video playing
            if self.videoPlaying :
                mouse.move(self.navi_btn_frame.winfo_rootx() + self.navi_btn_frame.winfo_width()/2 + 30, 
                        self.navi_btn_frame.winfo_rooty() + self.navi_btn_frame.winfo_height() + 20)
                
                if exit_btn_state == 1 :
                    self.closeVideo()
                
            else :
                if next_btn_state == 1 :
                    self.loadSelectionScreen(self.cur_page+1)
                elif prev_btn_state == 1 :
                    self.loadSelectionScreen(self.cur_page-1)

        self.window.after(1, self.readJoystick)


    def drawWidgets(self):
        # Draw bg, title, and navi button frames
        Frame(self.window).grid(row=0, column=0, sticky="ew")

        self.title = Label(self.window, text="Re-Imagining Breath", bg="black", fg="white", font="none 24 bold")
        self.title.grid(row=0)
        self.title.config(anchor=CENTER)
        
        # Page navigation buttons
        self.navi_btn_frame = LabelFrame(self.window, bg="black", borderwidth=0)
        self.navi_btn_frame.grid(row=2, column=0, columnspan=self.cols, sticky="news", pady=(0,5))
        self.btn_next_page = Button(self.navi_btn_frame, text='Next Page', bg="black", fg = "white")
        self.btn_prev_page = Button(self.navi_btn_frame, text='Previous Page', bg="black", fg = "white")
        self.btn_joystick = Button(self.navi_btn_frame, text='Enable Joystick', bg="black", fg = "white")  # joystick enable/disable
        
        # Video display and exit button
        self.video_screen = Label(self.window)  
        self.btn_exit = Button(self.window, text='Exit', bg="black", fg = "white")     


    def populateThumbnail(self):
        # Populate corresponding thumbnail for each video
        for video in self.videos:
            filename = video.split(".")[0]
            image = Image.open(f'./thumbnails/{filename}.png')

            w, h = image.size
            new_w  = (int)(self.screen_width/(self.cols))
            new_h = (int)(new_w * h / w )
            dim = max(new_w, new_h)

            image.thumbnail((dim, dim), Image.Resampling.LANCZOS)
            photo = ImageTk.PhotoImage(image)
            self.thumbnails.update({filename: photo})


    def makePages(self):
        rows = self.rows
        cols = self.cols
                
        # Pagination: for each page, place videos in a grid
        num_pages = ((self.total_videos) / (rows*cols)).__ceil__()
        cur_idx = 0
        
        for page_num in range(1, num_pages+1):
            new_page = {} 
            page_frame = LabelFrame(self.window, bg="black")

            # Place the videos row by column
            r, c = 0, 0
            total_videos_on_page = 0
            for video_idx in range(cur_idx, len(self.videos)):
                video_fn = self.videos[video_idx]
                filename = video_fn.split(".")[0]
                
                # Create a button and number label for each thumbnail so they can be clicked
                btn = Button(page_frame, image=self.thumbnails[f"{filename}"], command=lambda v=video_fn: self.Play(v, 30, mirror=False), bg="black")
                # lbl = Label(page_frame, bg="black", fg="white", text=f"{video_idx+1}", font="none 12 bold", anchor=E)
                btn.grid(row=r, column=c, sticky="news", padx=10, pady=10)
                # lbl.grid(row=r, column=c, sticky="ne", padx=(0,10), pady=(10,0))

                # Store the coordinates of each video to page
                new_page.update({(r,c): video_fn})

                # Increment rows and cols according to grid dimensions
                c += 1
                if (c == cols):
                    c = 0
                    r += 1
                if (r == rows):
                    r = 0

                # If page is full, stop placing videos
                total_videos_on_page += 1
                if (total_videos_on_page == rows*cols):
                    cur_idx = video_idx+1
                    break

            # Button thumbnails expand to fill up any extra space   
            page_frame.columnconfigure(tuple(range(cols)), weight=1)
            if (total_videos_on_page >= rows*cols or r >= rows-1):
                page_frame.rowconfigure(tuple(range(rows)), weight=1)
                            
            # Attach frame to new page, and add to pages
            new_page.update({"page": page_frame})
            self.pages.update({page_num: new_page})


    def loadSelectionScreen(self, page_num=1):
        if page_num < 1 or page_num > len(self.pages):
            return
  
        # Show the selected page
        self.cur_page = page_num
        selected_page = self.pages[page_num]["page"]
        selected_page.grid(row=1, column=0, sticky="news")  
        selected_page.tkraise()

        # Display navi buttons if more than 1 page
        if len(self.pages) > 1:
            self.showButtons()
        
        # Update the window
        self.window.update()
        
    # def thresholding_algo(y, lag, threshold, influence):
    #     signals = np.zeros(len(y))
    #     filteredY = np.array(y)
    #     avgFilter = [0]*len(y)
    #     stdFilter = [0]*len(y)
    #     avgFilter[lag - 1] = np.mean(y[0:lag])
    #     stdFilter[lag - 1] = np.std(y[0:lag])
    #     for i in range(lag, len(y)):
    #         if abs(y[i] - avgFilter[i-1]) > threshold * stdFilter [i-1]:
    #             if y[i] > avgFilter[i-1]:
    #                 signals[i] = 1
    #             else:
    #                 signals[i] = -1

    #             filteredY[i] = influence * y[i] + (1 - influence) * filteredY[i-1]
    #             avgFilter[i] = np.mean(filteredY[(i-lag+1):i+1])
    #             stdFilter[i] = np.std(filteredY[(i-lag+1):i+1])
    #         else:
    #             signals[i] = 0
    #             filteredY[i] = y[i]
    #             avgFilter[i] = np.mean(filteredY[(i-lag+1):i+1])
    #             stdFilter[i] = np.std(filteredY[(i-lag+1):i+1])

    #     return dict(signals = np.asarray(signals),
    #                 avgFilter = np.asarray(avgFilter),
    #                 stdFilter = np.asarray(stdFilter))
    
    def Play(self, filePath, framerate, mirror=False) :
        self.videoPlaying = True
        
        global cap
        cap = cv2.VideoCapture(f"./videos/{filePath}")
        filename = filePath.split(".")[0]

        # Show video playback tkinter frame
        self.video_screen.grid(column=0, row=0, columnspan=self.cols, rowspan=self.rows, sticky="news")
        self.video_screen.tkraise()
        self.btn_exit.grid(row=self.rows, column=0, pady=(10,0), sticky="news")
        self.btn_exit.configure(command= lambda: self.closeVideo())

        # Show video loading screen
        self.video_screen.config(text="Just a moment...", fg="white", bg="black", compound="bottom", image=self.thumbnails[f"{filename}"], height=self.screen_height, width=self.screen_width, font="none 16 bold")
        self.window.update()

        framecount = 0
        frames = []
        while cap.isOpened() and self.videoPlaying:
            self.window.update()
            success, frame = cap.read() 
            if success:
                resized = cv2.resize(frame, (self.screen_width, self.screen_height))
                cv2image = cv2.cvtColor(resized, cv2.COLOR_BGR2RGBA)
                img = Image.fromarray(cv2image)

                imgtk = ImageTk.PhotoImage(img)
                frames.append(imgtk)
                framecount += 1

            else:
                break

        # Frame data has been loaded
        frames.pop()
        frame = 0
        frames.reverse()
        revframes = frames.reverse()

        # init shared memory
        # Try except made it stuck on loading screen sometimes, not sure why.
        while 1:
            self.window.update() 
            try:
                shm_a = shared_memory.SharedMemory(name="ReimaginingBreath", size=10)
                break
            except:
                print("trying again")
                time.sleep(1)
        
        
        slopelist = []
      
        prevValue = 0
        prevBreathValue = 0
        slope = 0
        i = 0
        updateframe = 0
        updatetime = 3
        msperframe = int((1000//framerate)//self.playrate)
        self.window.update()

        while (self.videoPlaying):
         

            # Reads in byte array, convert that to integer using little endian.
            breathValue = int.from_bytes(bytes(shm_a.buf[:10]), 'little')
            # Only change speed if breathValue has changed
            # TODO when this is false, uses a ton of resources. FIx that? Limit shared mem reading rate would be good.
            if breathValue != prevBreathValue:
                # print("Breath value: ", breathValue, "prev: ", prevValue)

                if len(self.slopelist) >= 5:
                    alist = self.peaks.thresholding_algo(breathValue)
                    
                    # for value in alist:
                    print(alist)
                    # print("Alist: ", alist)
                    
                    # print("")


          
                self.slopelist.append(breathValue-prevValue)

                # if len(self.slopelist) >= 10:
                #     # Run algo with settings from above
                #     result = self.thresholding_algo(self.slopelist, lag=5, threshold=5, influence=1)

                #     pylab.subplot(211)
                #     pylab.plot(np.arange(1, len(y)+1), y)

                #     pylab.plot(np.arange(1, len(y)+1),
                #             result["avgFilter"], color="cyan", lw=2)

                #     pylab.plot(np.arange(1, len(y)+1),
                #             result["avgFilter"] + threshold * result["stdFilter"], color="green", lw=2)

                #     pylab.plot(np.arange(1, len(y)+1),
                #             result["avgFilter"] - threshold * result["stdFilter"], color="green", lw=2)

                #     pylab.subplot(212)
                #     pylab.step(np.arange(1, len(y)+1), result["signals"], color="red", lw=2)
                #     pylab.ylim(-1.5, 1.5)
                #     pylab.show()

                # if len(slopelist) > 6:
                #     slopelist.pop(0)
                for value in self.slopelist:
                    i += 1
                    slope += value  
                # if i < :
                slope = slope/i
                # else:
                    # slope = slope/6
                i = 0
                # if updateframe == updatetime:

                breathValue = int(breathValue)
                #self.playrate = (abs((slope - -10))/(10--10))*(1-.05)+.05
                # print(str((breathValue-prevValue)/updatetime))
                if alist>0:
                    self.reverse = True
                elif (alist)<0:
                    self.reverse = False
                
            
                    
                    
                    #msperframe = int((1000//framerate)//self.playrate)
                    # updateframe = 0
                prevValue = breathValue
            
                self.video_screen.config(text='', image=frames[frame])
                
                if self.reverse:
                    frame-=1
                    if (abs(frame) > framecount-2) and (frame < 0):
                        frame += framecount
                else:
                    frame+=1
                    if (abs(frame) > framecount-2) and (frame > 0):
                        frame -= framecount

                if cv2.waitKey(msperframe) and 0xFF == ord("q"):
                        self.videoPlaying = False
                        break
                
                if (abs(frame) == framecount-8):
                    #self.reverse = not self.reverse
                    frame = frame + framecount if frame < 0 else frame - framecount
                #prevValue = breathValue
                prevBreathValue = breathValue
                updateframe +=1
            self.window.update() 
            
        self.closeVideo()
        # Close shared mem
        shm_a.close()

            

    def closeVideo(self):
        self.videoPlaying = False
        cap.release()
        cv2.destroyAllWindows()

        self.btn_exit.grid_remove()
        self.video_screen.grid_remove()

        self.loadSelectionScreen(self.cur_page)


    def showButtons(self):
        num_pages = len(self.pages)

        self.btn_next_page.configure(command=lambda p=self.cur_page+1: self.loadSelectionScreen(p))
        self.btn_prev_page.configure(command=lambda p=self.cur_page-1: self.loadSelectionScreen(p))
        self.btn_joystick.configure(command=self.toggleJoystick)
        
        # Show navigation button(s) based on current page
        self.btn_next_page.grid_configure(row=0, column=(0 if self.cur_page == 1 else 1), pady=(10,0), columnspan=1, sticky="news")
        self.btn_prev_page.grid_configure(row=0, column=0, pady=(10,0), padx=(0,10), columnspan=1, sticky="news")
        self.btn_joystick.grid_configure(row=0, column=(2), sticky="news", padx=(10, 0), pady=(10,0))

        if self.cur_page == 1:    # first page
            self.btn_prev_page.grid_forget()
            self.btn_next_page.grid(columnspan=2)
        elif self.cur_page == num_pages:  #last page
            self.btn_next_page.grid_forget()
            self.btn_prev_page.grid(columnspan=2, padx=0)
        else:
            self.navi_btn_frame.columnconfigure(1, weight=1)

        self.navi_btn_frame.columnconfigure(0, weight=1)


    def toggleJoystick(self) :
        if self.joystick == None :
            try :
                self.joystick = joycon.JoyCon(pyjoycon.get_R_id())
                self.btn_joystick.config(text="Disable Joystick")
            except:
                # Show error dialog box if joycon not connected to bluetooth or not found
                print("No controller found.")
                top= Toplevel(self.window)
                top.geometry("200x100")
                top.title("Error")
                Label(top, text="No controller found.", fg="black").place(relx=0.5, rely=0.5, anchor=CENTER)

        else :
            self.joystick = None
            self.btn_joystick.config(text="Enable Joystick")


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument('--r', '--rows', default=3, type=int)
    parser.add_argument('--c', '--cols', '--columns', default=3, type=int)
    parser.add_argument('--use_joystick', default=0, type=int)

    args = parser.parse_args()
    gui = GUI(args.r, args.c, args.use_joystick)

    gui.run()
