clc; clearvars; close all

searchParams.startY = 2;
searchParams.endY = 0;
searchParams.startTheta = pi/4;
searchParams.endTheta = 3*pi/4;
searchParams.dy = .1;
searchParams.dtheta = .03;
searchParams.animateBool = 0;

grip = gripper('basePosition', [-3.5 2.5], 'parallelLinkLength', 1, 'endEffectorLength', 4);

shapes = ["circle", "square", "sine"];
figure();
sgtitle("Detection of Obstacle Shape");
for i = 1:numel(shapes)
    
    obst = obstacle("shape", shapes(i));
    subplot(2, 2, i)
    grip.predictObstacle(obst, searchParams)
    hold on
    obst.drawObstacle;
    hold off
    axis padded
end