function searchParams = getSearchParams

    searchParams.startY = 2;
    searchParams.endY = 0;
    searchParams.startTheta = pi/4;
    searchParams.endTheta = 3*pi/4;
    searchParams.dy = .1;
    searchParams.dtheta = .05;

    searchParams.animateBool = 1;
    searchParams.saveBool = 0;

    if searchParams.saveBool
        searchParams.animateBool = 1;
    end
end