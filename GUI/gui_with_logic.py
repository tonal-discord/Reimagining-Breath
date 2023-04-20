
import os
import sys
import cv2
import pyjoycon, joycon, mouse
from tkinter import *
from PIL import Image, ImageTk
from multiprocessing import shared_memory

class GUI:

    max_rows = 10
    max_cols = 10

    def __init__(self, rows=3, cols=3):
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
        # self.joystick = joycon.JoyCon(pyjoycon.get_R_id())

        self.thumbnails = {}
        self.pages = {}
        self.cur_page = 1
        self.videoPlaying = False

        # Video playback logic variables
        self.playrate = .5
        self.reverse = False

        # Drawing & preprocessing
        self.drawWidgets()
        self.populateThumbnail()
        self.makePages()
 

    def run(self):
        # Load page number 1
        self.loadSelectionScreen(1)

        # Start reading joystick
        # self.window.after(1, self.readJoystick)

        # Run the GUI window
        self.window.mainloop()


    def readJoystick(self): 
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
        
    
    def Play(self, filePath, framerate, mirror=False) :
        self.videoPlaying = True
        
        global cap
        cap = cv2.VideoCapture(f"./videos/{filePath}")

        # Show video playback tkinter frame
        self.video_screen.grid(column=0, row=0, columnspan=self.cols, rowspan=self.rows, sticky="news")
        self.video_screen.tkraise()
        self.btn_exit.grid(row=self.rows, column=0, pady=(10,0), sticky="news")
        self.btn_exit.configure(command= lambda: self.closeVideo())

        # Show video loading screen
        self.video_screen.config(text="Just a moment...", fg="white", bg="black", height=self.screen_height, width=self.screen_width, image='', font="none 16 bold")
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
        shm_a = shared_memory.SharedMemory(name="ReimaginingBreath", size=10)
        # TODO error handle this
        slopelist = []
        prevValue = 0
        slope = 0
        i = 0
        updateframe = 0
        updatetime = 2
        msperframe = int((1000//framerate)//self.playrate)
        while (self.videoPlaying):
             
            # Reads in byte array, convert that to integer using little endian.
            breathValue = int.from_bytes(bytes(shm_a.buf[:10]), 'little')
            print("Breath value: ", breathValue)
            slopelist.append(breathValue-prevValue))
            if len(slopelist) >=6:
                slopelist.pop(0)
            for value in slopelist:
                i ++
                slope += i
            if i < 6:
                slope = slope/i
            else:
                slope = slope/6
            if updateframe == updatetime:

                breathValue = int(breathValue)
                #self.playrate = (abs((slope - -10))/(10--10))*(1-.05)+.05
                print(str((breathValue-prevValue)/updatetime))
                if slope>0:
                   self.reverse = True
                elif ((breathValue-prevValue)/updatetime)<0:
                    self.reverse = False
               
                
                
                msperframe = int((1000//framerate)//self.playrate)
                updateframe = 0
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
        
        # Show navigation button(s) based on current page
        self.btn_next_page.grid_configure(row=0, column=(0 if self.cur_page == 1 else 1), pady=(10,0), columnspan=1, sticky="news")
        self.btn_prev_page.grid_configure(row=0, column=0, pady=(10,0), padx=(0,10), columnspan=1, sticky="news")

        if self.cur_page == 1:    # first page
            self.btn_prev_page.grid_forget()
            self.btn_next_page.grid(columnspan=self.cols)
        elif self.cur_page == num_pages:  #last page
            self.btn_next_page.grid_forget()
            self.btn_prev_page.grid(columnspan=self.cols, padx=0)
        else:
            self.navi_btn_frame.columnconfigure(1, weight=1)

        self.navi_btn_frame.columnconfigure(0, weight=1)


if __name__ == "__main__":
    global gui

    if len(sys.argv) > 2 and sys.argv[1].isdigit() and sys.argv[2].isdigit():
        gui = GUI(int(sys.argv[1]), int(sys.argv[2])) 
    else:
        gui = GUI()
    
    gui.run()
