import numpy as np
import math
import matplotlib.pyplot as plt
import matplotlib.animation as animation

def drawBody():
    ax.plot([0, 0], [-150, 150], [0, 0], color="blue")
    ax.plot([-100, 100], [-150, -150], [0, 0], color="blue")
    ax.plot([-100, 100], [150, 150], [0, 0], color="blue")

class Trajectory:
    def __init__(self, step_length_init, step_height_init, back_step_depth_init, phase_init):
        self.step_length = step_length_init
        self.step_height = step_height_init
        self.back_step_depth = back_step_depth_init
        self.y = 0
        self.z = 0
        self.interpolations = 40
        self.phase = phase_init
        self.swing = phase_init == 1  # True if phase is 1, otherwise False

    def interpolate_next(self, speed):
        # speed should be between 0 and 100
        factor = self.step_length / (self.interpolations - (speed / 100.0 * 20))
        
        if self.swing:
            self.y += factor
            
            if self.y >= self.step_length:
                self.y = self.step_length
                self.swing = False
            
            self.z = self.step_height * math.sin(2 * math.pi / (self.step_length * 2) * self.y)
        else:
            self.y -= factor
            
            if self.y <= 0:
                self.y = 0
                self.swing = True
            
            self.z = -self.back_step_depth * math.sin(2 * math.pi / (self.step_length * 2) * self.y)

    def get_y(self):
        return self.y

    def get_z(self):
        return self.z
    

def animate(frame):
    ax.cla()
    ax.set_xlim(-200,200)
    ax.set_ylim(-200,200)
    ax.set_zlim(-200,200)
    ax.set_xlabel('X axis')
    ax.set_ylabel('Y axis')
    ax.set_zlabel('Z axis')
    
    drawBody()
    FR_trajectory.interpolate_next(0)
    ax.plot([0, -100], [0, FR_trajectory.get_y()], [0, FR_trajectory.get_z()])
    
    

fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')


FR_trajectory = Trajectory(100, 100, 5, 1)
    
ani = animation.FuncAnimation(fig, animate, frames=200, interval=50)

# Show the plot
plt.show()
    
