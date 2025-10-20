classdef gripperSet
    %UNTITLED5 Summary of this class goes here
    %   Detailed explanation goes here

    properties
        lhs
        rhs
    end

    methods
        function obj = gripperSet(lhs, rhs)
            obj.lhs = lhs;
            obj.rhs = rhs;
        end

        function detectObstacle(obj, obstacle, searchParams)

            if searchParams.saveBool
                searchParams.animateBool = 1;
            end

            obstacle.drawObstacle();

            startY = searchParams.startY;
            endY = searchParams.endY;
            startTheta = searchParams.startTheta;
            endTheta = searchParams.endTheta;
            dy = searchParams.dy;
            dtheta = searchParams.dtheta;

            searchYs = startY:-dy:endY;
            searchThetas = startTheta:dtheta:endTheta;

            contactPositions_lhs = [];
            contactPositions_rhs = [];

            if searchParams.saveBool
                writerObj = VideoWriter('animation.mp4');
                writerObj.FrameRate = 30;
                open(writerObj);
            end


            for i = 1:numel(searchYs)
                obj.lhs.basePosition(2) = searchYs(i);
                obj.rhs.basePosition(2) = searchYs(i);
                ctr = 1;
                contact_lhs = 0;
                contact_rhs = 0;

                % Both sides same time
                while ~(contact_lhs && contact_rhs) && (ctr <= numel(searchThetas))
                    % current angle
                    cur_theta = searchThetas(ctr);

                    % Define positions of each mechanism if they haven't
                    % made contact yet
                    if ~contact_lhs
                        [positions_lhs,tip_lhs] = obj.lhs.definePositions(cur_theta);
                        endEffectorPosition_lhs = positions_lhs(end, :);
                        [in_lhs, on_lhs] = inpolygon(tip_lhs(1,:), tip_lhs(2,:), obstacle.coords(1,:), obstacle.coords(2,:));
                        if sum([in_lhs, on_lhs]) > 0
                            contact_lhs = 1;
                            contact_lhs_theta = cur_theta;
                            contactPositions_lhs = [contactPositions_lhs;endEffectorPosition_lhs];
                        end
                    end

                    if ~contact_rhs
                        [positions_rhs,tip_rhs] = obj.rhs.definePositions(cur_theta);
                        endEffectorPosition_rhs = positions_rhs(end, :);
                        [in_rhs, on_rhs] = inpolygon(tip_rhs(1,:), tip_rhs(2,:), obstacle.coords(1,:), obstacle.coords(2,:));
                        if sum([in_rhs, on_rhs]) > 0
                            contact_rhs = 1;
                            contact_rhs_theta = cur_theta;
                            contactPositions_rhs = [contactPositions_rhs;endEffectorPosition_rhs];
                        end
                    end

                    ctr = ctr + 1;

                    if searchParams.animateBool
                        if ~contact_lhs
                            h_lhs = obj.lhs.plotPosition(cur_theta);
                        else
                            h_lhs = obj.lhs.plotPosition(contact_lhs_theta);
                        end

                        if ~contact_rhs
                            h_rhs = obj.rhs.plotPosition(cur_theta);
                        else
                            h_rhs = obj.rhs.plotPosition(contact_rhs_theta);
                        end

                        drawnow;
                        if searchParams.saveBool
                            writeVideo(writerObj, getframe(gcf));
                        end
                        delete([h_lhs,h_rhs]);

                    end
                end
            end
            if searchParams.saveBool
                close(writerObj);
            end
        end

        function predictObstacle(obj, obstacle, searchParams)
            endEffectorPositions_lhs = obj.lhs.detectObstacle(obstacle, searchParams);
            endEffectorPositions_rhs = obj.rhs.detectObstacle(obstacle, searchParams);
            x_vals_lhs = endEffectorPositions_lhs(:, 1);
            y_vals_lhs = endEffectorPositions_lhs(:, 2);
            x_vals_rhs = endEffectorPositions_rhs(:, 1);
            y_vals_rhs = endEffectorPositions_rhs(:, 2);

            gca;
            hold on
            scatter(x_vals_lhs, y_vals_lhs, 'x', 'b');
            scatter(x_vals_rhs, y_vals_rhs, 'x', 'r');
            axis equal
        end
    end
end