classdef gripper
    %UNTITLED2 Summary of this class goes here
    %   Detailed explanation goes here

    properties
        baseLinkLength = 1;
        parallelLinkLength = 3;
        endEffectorLength = 5;

        basePosition = [0,0];
        handedness = "l"
    end

    methods
        function obj = gripper(varargin)


            p = inputParser();
            addParameter(p, 'baseLinkLength', obj.baseLinkLength);
            addParameter(p, 'parallelLinkLength', obj.parallelLinkLength);
            addParameter(p, 'endEffectorLength', obj.endEffectorLength);
            addParameter(p, 'basePosition', obj.basePosition);
            addParameter(p, 'handedness', obj.handedness);

            parse(p, varargin{:});

            obj.baseLinkLength = p.Results.baseLinkLength;
            obj.parallelLinkLength = p.Results.parallelLinkLength;
            obj.endEffectorLength = p.Results.endEffectorLength;
            obj.basePosition = p.Results.basePosition;
            obj.handedness = p.Results.handedness;



        end

        function [positions, tip] = definePositions(obj,theta)
            % o: origin; b: base; p = outside parallel; e: end effector
            r_ob1 = obj.basePosition;
            r_b1b2 = [-obj.baseLinkLength, 0];
            r_b2p = -[obj.parallelLinkLength*cos(theta), obj.parallelLinkLength*sin(theta)];
            r_pe = [obj.endEffectorLength, 0];

            if obj.handedness == 'r'
                r_b1b2 = -r_b1b2;
                r_b2p(1) = -r_b2p(1);
                r_pe = -r_pe;
            end

            b1 = r_ob1;
            b2 = b1 + r_b1b2;
            p = b2 + r_b2p;
            e = p + r_pe;

            positions = [b1; b2; p; e];

            % Tip
            thetas = linspace(-pi/2, pi/2, 200);
            r = .2;
            tip_x = r*cos(thetas);
            tip_y = r*sin(thetas);

            if obj.handedness == 'r'
                tip_x = -tip_x;
            end

            tip = [tip_x; tip_y] + e';
        end

        function handles = plotPosition(obj, theta)
            eps = 0.1;
            [positions, tip] = obj.definePositions(theta);
            % base link
            b1x = positions(1, 1);
            b1y = positions(1, 2);
            b2x = positions(2, 1);
            b2y = positions(2, 2);

            base_x = [b1x, b1x, b2x, b2x];
            base_y = [b1y + eps, b2y - eps, b2y - eps, b1y + eps];

            % Outside Parallel Linkage
            p2x = positions(3, 1);
            p2y = positions(3, 2);

            par_x = [p2x, b2x];
            par_y = [p2y, b2y];

            % End effecter
            ex = positions(4, 1);
            ey = positions(4, 2);

            e_x = [p2x, p2x, ex, ex];
            e_y = [p2y-eps, ey+eps, ey+eps, p2y-eps];

            tip_x = tip(1, :);
            tip_y = tip(2, :);

            gca;
            hold on
            h1 = fill(base_x, base_y, 'k');
            h2 = plot(par_x, par_y, 'LineWidth', 2, 'Color', 'r');
            if obj.handedness == 'l'
                h3 = plot(par_x + obj.baseLinkLength, par_y, 'LineWidth', 2, 'Color', 'b');
            else
                h3 = plot(par_x - obj.baseLinkLength, par_y, 'LineWidth', 2, 'Color', 'b');
            end
            h4 = fill(e_x, e_y, 'r');
            h5 = fill(tip_x, tip_y, 'k');
            axis([-7 7 -5 5]);

            handles = [h1, h2, h3, h4, h5];
        end

        function animateGripper(obj, thetas)
            gca;

            for i = 1:length(thetas)
                theta = thetas(i);
                hs = obj.plotPosition(theta);
                drawnow
                delete(hs);

            end
        end

        function contactPositions = detectObstacle(obj, obstacle, searchParams)
            obstacle.drawObstacle();

            startY = searchParams.startY;
            endY = searchParams.endY;
            startTheta = searchParams.startTheta;
            endTheta = searchParams.endTheta;
            dy = searchParams.dy;
            dtheta = searchParams.dtheta;

            searchYs = startY:-dy:endY;
            searchThetas = startTheta:dtheta:endTheta;

            contactPositions = [];


            for i = 1:numel(searchYs)
                obj.basePosition(2) = searchYs(i);
                ctr = 1;
                contact = 0;
                while ~contact && ctr <= numel(searchThetas)
                    cur_theta = searchThetas(ctr);
                    [positions,tip] = obj.definePositions(cur_theta);
                    endEffectorPosition = positions(end, :);
                    [in, on] = inpolygon(tip(1,:), tip(2,:), obstacle.coords(1,:), obstacle.coords(2,:));
                    if (sum(in) + sum(on)) > 0
                        contact = 1;
                        contactPositions = [contactPositions;endEffectorPosition];
                    end
                    ctr = ctr + 1;
                    if searchParams.animateBool
                        hs = obj.plotPosition(cur_theta);
                        drawnow;
                        pause(.2);
                        delete(hs);
                    end
                end

            end
        end

        function predictObstacle(obj, obstacle, searchParams)
            endEffectorPositions = obj.detectObstacle(obstacle, searchParams);
            x_vals = endEffectorPositions(:, 1);
            y_vals = endEffectorPositions(:, 2);
            gca;
            if obj.handedness == 'r'
                color = 'r';
            else
                color = 'b';
            end
            scatter(x_vals, y_vals, 'x', color);
            axis equal
        end

    end
end