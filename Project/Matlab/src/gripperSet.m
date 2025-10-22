classdef gripperSet

    properties
        lhs
        rhs
    end

    methods
        function obj = gripperSet(lhs, rhs)
            obj.lhs = lhs;
            obj.rhs = rhs;
        end

        function [contactPosition_lhs, contactPosition_rhs] = searchHeight(obj, obstacle, searchParams, height)
            
            %** Search for contact with the obstacle at a given height

            obstacle.drawObstacle();

            startTheta = searchParams.startTheta;
            endTheta = searchParams.endTheta;
            dtheta = searchParams.dtheta;

            searchThetas = startTheta:dtheta:endTheta;

            contactPosition_lhs = [];
            contactPosition_rhs = [];

            obj.lhs.basePosition(2) = height;
            obj.rhs.basePosition(2) = height;
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
                    [contact_lhs, contact_lhs_theta, contactPosition_lhs] = detectContact(obj, cur_theta, 'l', obstacle);
                end

                if ~contact_rhs
                    [contact_rhs, contact_rhs_theta, contactPosition_rhs] = detectContact(obj, cur_theta, 'r', obstacle);
                end

                ctr = ctr + 1;

                % This will freeze the arm in the same position if it has
                % already come into contact with the obstacle
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

        function [contact, contact_theta, contactPosition] = detectContact(obj, theta, hand, obstacle)
            %** detect contact between the hand and the obstacle

            contact = 0;
            contact_theta = [];
            contactPosition = [];

            if hand == 'r'
                [positions,tip] = obj.rhs.definePositions(theta);
            elseif hand == 'l'
                [positions,tip] = obj.lhs.definePositions(theta);
            end

            % Check for collisions and return contact, theta, and position
            % if so
            endEffectorPosition = positions(end, :);
            [in, on] = inpolygon(tip(1,:), tip(2,:), obstacle.coords(1,:), obstacle.coords(2,:));
            if sum([in, on]) > 0
                contact= 1;
                contact_theta = theta;
                contactPosition = endEffectorPosition;
            end

        end
        function [contactPositions_lhs, contactPositions_rhs] = detectObstacle(obj, obstacle, searchParams)

            if searchParams.saveBool
                searchParams.animateBool = 1;
            end

            startY = searchParams.startY;
            endY = searchParams.endY;
            dy = searchParams.dy;

            searchYs = startY:-dy:endY;

            contactPositions_lhs = [];
            contactPositions_rhs = [];

            if searchParams.saveBool
                writerObj = VideoWriter('animation.mp4');
                writerObj.FrameRate = 30;
                open(writerObj);
            end

            for i = 1:numel(searchYs)
                [l, r] = searchHeight(obj, obstacle, searchParams, searchYs(i));
                contactPositions_lhs = [contactPositions_lhs; l];
                contactPositions_rhs = [contactPositions_rhs; r];
            end

        end

        function predictObstacle(obj, obstacle, searchParams)
            [endEffectorPositions_lhs, endEffectorPositions_rhs] = obj.detectObstacle(obstacle, searchParams);

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