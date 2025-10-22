clc; clearvars; close all

addpath('src');

%{
Would make more sense for it to start on the ground then make its way up.
That way, whenever it reaches a local maxima on diameter, it'll just stop

Also, can add feature where if they run into each other they'll know
they've gone over the object.
%}

lh_grip = gripper('basePosition', [-3.5 2.5], 'parallelLinkLength', 1, 'endEffectorLength', 4);
rh_grip = gripper('handedness', 'r', 'basePosition', [4 2.5], 'parallelLinkLength', 1, 'endEffectorLength', 4);


%% Simulataneous Grip
searchParams = getSearchParams();
hand = gripperSet(lh_grip, rh_grip, searchParams);

% ** Multiple shapes **
shapes = ["circle", "square", "sine"];
figure();
sgtitle("Detection of Obstacle Shape");
for i = 1:numel(shapes)

    obst = obstacle("shape", shapes(i));
    subplot(2, 2, i)
    hold on
    hand.predictObstacle(obst);
    obst.drawObstacle;
    hold off
    axis padded
end


%** Single obstacle **
% obst = obstacle("shape", "square");

% hand.detectObstacle(obst, searchParams);
% hand.predictObstacle(obst, searchParams);