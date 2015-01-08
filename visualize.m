
clc;

A = importdata('logs/test8.txt',',');
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


% R = reshape(rho, 30, 30);
figure
contourf(R);
colorbar
figure;
quiver(x, y, u, v);

figure

S(S(:, :) == 0) = nan;
imagesc(S ./ 0.05);
colorbar
axis equal off; drawnow