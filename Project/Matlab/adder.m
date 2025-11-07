classdef adder
    %UNTITLED2 Summary of this class goes here
    %   Detailed explanation goes here

    properties
        a
        b
    end

    methods
        function obj = adder(a, b)
            %UNTITLED2 Construct an instance of this class
            %   Detailed explanation goes here
            obj.a = a;
            obj.b = b; 
        end

        function c = add(obj)
            %METHOD1 Summary of this method goes here
            %   Detailed explanation goes here
            c = obj.a + obj.b;
        end

        function sayHello(obj)
            disp("Hello");
        end

        function output = compare(obj, input)
            c = obj.add();
          
            if input > c
                output = 1;
            else
                output = 0;
            end
        end
    end
end