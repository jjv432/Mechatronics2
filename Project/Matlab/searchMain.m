clc; clearvars; close all

%{
Would make more sense for it to start on the ground then make its way up.
That way, whenever it reaches a local maxima on diameter, it'll just stop

Also, can add feature where if they run into each other they'll know
they've gone over the object.
%}
searchParams.startY = 1.5;
searchParams.endY = 0;
searchParams.startTheta = pi/4;
searchParams.endTheta = 3*pi/4;
searchParams.dy = .1;
searchParams.dtheta = .05;
searchParams.animateBool = 0;

lh_grip = gripper('basePosition', [-3.5 2.5], 'parallelLinkLength', 1, 'endEffectorLength', 4);
rh_grip = gripper('handedness', 'r', 'basePosition', [4 2.5], 'parallelLinkLength', 1, 'endEffectorLength', 4);

%%
% rh_grip.plotPosition(pi/2);
% lh_grip.plotPosition(pi/2);

% shapes = ["circle", "square", "sine"];
% figure();
% sgtitle("Detection of Obstacle Shape");
% for i = 1:numel(shapes)
% 
%     obst = obstacle("shape", shapes(i));
%     subplot(2, 2, i)
%     hold on
%     rh_grip.predictObstacle(obst, searchParams)
%     lh_grip.predictObstacle(obst, searchParams)
%     obst.drawObstacle;
%     hold off
%     axis padded
% end

%% Simulataneous Grip
obst = obstacle("shape", "square");
searchParams.animateBool = 1;
hand = gripperSet(lh_grip, rh_grip);
hand.detectObstacle(obst, searchParams);