import numpy as np
import math
import matplotlib.pyplot as plt
import matplotlib.animation as animation

#declare constants
STEP_LENGTH = 100
STEP_HEIGHT = 60
BACK_STEP_DEPTH = 10
GROUND_DEPTH = 150

FR_abadPos = [50, 150, 0]
FL_abadPos = [-50, 150, 0]
BR_abadPos = [50, -150, 0]
BL_abadPos = [-50, -150, 0]


L1 = 100
L2 = 100
L3 = 50

def drawBody():
    ax.plot([0, 0], [-150, 150], [0, 0], color="blue")
    ax.plot([FR_abadPos[0], FL_abadPos[0]], [FR_abadPos[1], FL_abadPos[1]], [FR_abadPos[2], FL_abadPos[2]], color="blue")
    ax.plot([BR_abadPos[0], BL_abadPos[0]], [BR_abadPos[1], BL_abadPos[1]], [BR_abadPos[2], BL_abadPos[2]], color="blue")

class Trajectory:
    def __init__(self, phase_init):
        self.phase = phase_init
        if self.phase == "swing":
            self.y = 0
            self.z = 0
        elif self.phase == "support":
            self.y = STEP_LENGTH
            self.z = 0
        self.interpolations = 40
    def interpolate_next(self, speed):
        # speed should be between 0 and 100
        factor = STEP_LENGTH / (self.interpolations - (speed / 100.0 * 35))
        if self.phase == "swing":
            self.y += factor
            if self.y >= STEP_LENGTH:
                self.y = STEP_LENGTH
                self.phase = "support"
            self.z = STEP_HEIGHT * math.sin(2 * math.pi / (STEP_LENGTH * 2) * self.y)
        elif self.phase == "support":
            self.y -= factor
            if self.y <= 0:
                self.y = 0
                self.phase = "swing"
            self.z = -BACK_STEP_DEPTH * math.sin(2 * math.pi / (STEP_LENGTH * 2) * self.y)
    def get_y(self):
        return self.y
    def get_z(self):
        return self.z
    
def solve_IK(targetPos, abadPos):
    x = targetPos[0]-abadPos[0]
    y = targetPos[1]-abadPos[1]
    z = targetPos[2]-abadPos[2]
    d = math.sqrt(math.pow(y, 2) + math.pow(z, 2))
    alpha = math.acos((math.pow(L1, 2) + math.pow(L2, 2) - math.pow(d, 2)) / (2 * L1 * L2))
    theta = math.pi/2 + math.atan(y / z) - math.asin(L2 * math.sin(alpha) / d)
    phi = alpha - theta
    row = math.asin(L3 / math.sqrt(math.pow(z, 2) + pow(x, 2)))
    try: 
        omega = row - math.atan(z / x)
    except ZeroDivisionError:
        omega = row - math.pi/2
    
    return omega, theta, phi #abad angle, hip angle, knee angle

    
def animate(frame):
    ax.cla()
    ax.set_xlim(-200,200)
    ax.set_ylim(-200,200)
    ax.set_zlim(-200,200)
    ax.set_xlabel('X axis')
    ax.set_ylabel('Y axis')
    ax.set_zlabel('Z axis')
    
    drawBody() 
    FR_trajectory.interpolate_next(100)
    FL_trajectory.interpolate_next(100)
    BR_trajectory.interpolate_next(100)
    BL_trajectory.interpolate_next(100)
    
    FR_targetPos = translateTrajectory([0, FR_trajectory.get_y(), FR_trajectory.get_z()], "FR")
    FL_targetPos = translateTrajectory([0, FL_trajectory.get_y(), FL_trajectory.get_z()], "FL")
    BR_targetPos = translateTrajectory([0, BR_trajectory.get_y(), BR_trajectory.get_z()], "BR")
    BL_targetPos = translateTrajectory([0, BL_trajectory.get_y(), BL_trajectory.get_z()], "BL")
    
    FR_angles = solve_IK(FR_targetPos, FR_abadPos)
    FL_angles = solve_IK(FL_targetPos, FL_abadPos)
    BR_angles = solve_IK(BR_targetPos, BR_abadPos)
    BL_angles = solve_IK(BL_targetPos, BL_abadPos)
    
    #forward kinematics to find hip joint pos
    FR_hipPos = [FR_abadPos[0] - L3*math.cos(FR_angles[0]), FR_abadPos[1], FR_abadPos[2] + L3*math.sin(FR_angles[0])]
    FL_hipPos = [FL_abadPos[0], FL_abadPos[1] + L3*math.sin(FL_angles[0]), FL_abadPos[2] - L3*math.cos(FL_angles[0])]
    BR_hipPos = [BR_abadPos[0], BR_abadPos[1] + L3*math.sin(BR_angles[0]), BR_abadPos[2] - L3*math.cos(BR_angles[0])]
    BL_hipPos = [BL_abadPos[0], BL_abadPos[1] + L3*math.sin(BL_angles[0]), BL_abadPos[2] - L3*math.cos(BL_angles[0])]
    
    #draw pelvis
    ax.plot([FR_abadPos[0], FR_hipPos[0]], [FR_abadPos[1], FR_hipPos[1]], [FR_abadPos[2], FR_hipPos[2]])
    ax.plot([FL_abadPos[0], FL_hipPos[0]], [FL_abadPos[1], FL_hipPos[1]], [FL_abadPos[2], FL_hipPos[2]])
    ax.plot([BR_abadPos[0], BR_hipPos[0]], [BR_abadPos[1], BR_hipPos[1]], [BR_abadPos[2], BR_hipPos[2]])
    ax.plot([BL_abadPos[0], BL_hipPos[0]], [BL_abadPos[1], BL_hipPos[1]], [BL_abadPos[2], BL_hipPos[2]])
    
    #ax.plot([FR_trajectory[0], ], [FR_trajectory[1], ], [FR_trajectory[2], ])
    
    ax.plot([0, FR_targetPos[0]], [0, FR_targetPos[1]], [0, FR_targetPos[2]])
    ax.plot([0, FL_targetPos[0]], [0, FL_targetPos[1]], [0, FL_targetPos[2]])
    ax.plot([0, BR_targetPos[0]], [0, BR_targetPos[1]], [0, BR_targetPos[2]])
    ax.plot([0, BL_targetPos[0]], [0, BL_targetPos[1]], [0, BL_targetPos[2]])
    
    
def translateTrajectory(pos, leg):
    if leg == "FR":
        return [FR_abadPos[0]+50, pos[1]+FR_abadPos[1]-STEP_LENGTH/2, pos[2]-GROUND_DEPTH]
    elif leg == "FL":
        return [FL_abadPos[0]-50, pos[1]+FL_abadPos[1]-STEP_LENGTH/2, pos[2]-GROUND_DEPTH]
    elif leg == "BR":
        return [BR_abadPos[0]+50, pos[1]+BR_abadPos[1]-STEP_LENGTH/2, pos[2]-GROUND_DEPTH]
    elif leg == "BL":
        return [BL_abadPos[0]-50, pos[1]+BL_abadPos[1]-STEP_LENGTH/2, pos[2]-GROUND_DEPTH]
    return

#main program below
fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')

FR_trajectory = Trajectory("swing")
FL_trajectory = Trajectory("support")
BR_trajectory = Trajectory("support")
BL_trajectory = Trajectory("swing")

ani = animation.FuncAnimation(fig, animate, frames=200, interval=50)

# Show the plot
plt.show()
    
