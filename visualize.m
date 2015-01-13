clc;
figure
nx = 100;
ny = 100;
totalIterations = 10000;
max_rho = 1;
for iteration = 0:15:totalIterations
    iteration
    filename = sprintf('logs/D2Q9_POINT_3_%dx%d_%d_%d.txt', nx, ny, totalIterations, iteration);
    A = importdata(filename,',');
    x = A(:, 1); y = A(:, 2); rho = A(:, 3); u = A(:, 4); v = A(:, 5);
    n = length(x);
    max_x = max(x) + 1;
    max_y = max(y) + 1;
    R = zeros(max_y, max_x);
    U = zeros(max_y, max_x);
    V = zeros(max_y, max_x);
    S = zeros(max_y, max_x);

    for i = 1:n
        R(y(i) + 1, x(i) + 1) = rho(i);
        U(y(i) + 1, x(i) + 1) = u(i);
        V(y(i) + 1, x(i) + 1) = v(i);
        S(y(i) + 1, x(i) + 1) = u(i).^2 + v(i).^2;

    end


    max_rho = max([max_rho, max(rho)]);
    max_rho = 1.05;

    % R = reshape(rho, 30, 30);
    contourf(R);
    caxis([1, max_rho]);
    % quiver(x, y, u, v);
    title(iteration);

    % contourf(sqrt(S));
    colorbar
    pause(0.1)

end
% figure;

% figure

% colorbar
% axis equal off; drawnow