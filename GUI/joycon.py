import pyjoycon
import mouse

#Functions to enable on JoyCon
class JoyCon :

    class JoyConInstance(
        pyjoycon.ButtonEventJoyCon,
        pyjoycon.JoyCon
    ): pass

    def __init__(self, id) :

        # Assigns controller ID's(currently only Right JoyCon)
        self.id = id

        # Assign variables to JoyCons
        self.joycon = self.JoyConInstance(*self.id)

        # Track button click statuses
        self.prev_r_stick_sw_state = 0

        # Lower value = higher sensitivity
        self.sensitivity = 200

        # Neutral position is H: 2169 V:1863 
        # H Limits: 975, 3420
        # V Limits: 747, 2989
        self.neutral_horizontal = 2169
        self.neutral_vertical = 1863
        
        print("joycon connected")


    def read(self) :

        #Get value for Horizontal and Vertical movement
        move_x = (self.joycon.get_stick_right_horizontal() - self.neutral_horizontal)
        move_y = -(self.joycon.get_stick_right_vertical() - self.neutral_vertical)

        #Create threshold/dead zone before movement is allowed
        if -50 < move_x < 50:
            move_x = 0
        if -50 < move_y < 50:
            move_y = 0

        # Move mouse/cursor
        mouse.move(move_x/self.sensitivity, 
                   move_y/self.sensitivity, 
                   absolute=False, 
                   duration=0.001)

        # Check for stick clicks
        r_stick_sw_state = self.joycon.get_button_r_stick()
        if self.prev_r_stick_sw_state != r_stick_sw_state :
            if r_stick_sw_state == 1 :
                mouse.click(button="left")

        # Get status of button clicks
        prev_btn_state, next_btn_state, exit_btn_state = 0, 0, 0

        for event_type, status in self.joycon.events(): 
            # print(event_type, status) 

            if status == 1:
                if event_type == "a" : 
                    next_btn_state = 1
                elif event_type == "y" :
                    prev_btn_state = 1
                # elif event_type == "x":
                #     exit_btn_state = 1
                elif event_type in ["b", "r", "zr"] :
                    mouse.click(button='left')

        self.prev_r_stick_sw_state = r_stick_sw_state

        return prev_btn_state, next_btn_state, exit_btn_state