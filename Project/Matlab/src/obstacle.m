classdef obstacle

    properties
        shape = 'square'
        coords
    end

    methods
        function obj = obstacle(varargin)
            p = inputParser();
            addParameter(p, "shape", obj.shape);

            parse(p, varargin{:});

            obj.shape = p.Results.shape;
            obj = obj.makeCoords();
        end

        function obj = makeCoords(obj)

            if obj.shape == "square"
                obj.coords = [-1 -1 1 1; -1 1 1 -1]; 
            elseif obj.shape == "circle"
                thetas = linspace(0, 2*pi, 500);
                r = 1;
                obj.coords = [r*cos(thetas); r*sin(thetas)];
            elseif obj.shape == "sine"
                y_vals = linspace(0, 1, 100);
                x_vals = .25*sin(10*y_vals);

                obj.coords = [x_vals, flip(x_vals) + 1; y_vals, flip(y_vals)];
            end

        end
        function drawObstacle(obj)
            
            gca;
            fill(obj.coords(1, :), obj.coords(2, :), 'm');

        end
    end
end