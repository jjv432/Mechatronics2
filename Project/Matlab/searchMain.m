clc; clearvars; close all

searchParams.startY = 2;
searchParams.endY = 0;
searchParams.startTheta = pi/4;
searchParams.endTheta = 3*pi/4;
searchParams.dy = .1;
searchParams.dtheta = .03;
searchParams.animateBool = 0;

lh_grip = gripper('basePosition', [-3.5 2.5], 'parallelLinkLength', 1, 'endEffectorLength', 4);
rh_grip = gripper('handedness', 'r', 'basePosition', [4 2.5], 'parallelLinkLength', 1, 'endEffectorLength', 4);

% rh_grip.plotPosition(pi/2);
% lh_grip.plotPosition(pi/2);

shapes = ["circle", "square", "sine"];
figure();
sgtitle("Detection of Obstacle Shape");
for i = 1:numel(shapes)

    obst = obstacle("shape", shapes(i));
    subplot(2, 2, i)
    hold on
    rh_grip.predictObstacle(obst, searchParams)
    lh_grip.predictObstacle(obst, searchParams)
    obst.drawObstacle;
    hold off
    axis padded
end