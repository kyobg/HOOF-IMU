% Coordinates of the joints (A, B, C)
A = [5, 10]; % Replace with actual coordinates (hip)
B = [5.1, 5.75]; % Replace with actual coordinates (knee)
C = [5.089, 1]; % Replace with actual coordinates (ankle)

% Calculate the vectors AB and BC
AB = A - B;
BC = C - B;

% Calculate the lengths of sides AB and BC
lengthAB = norm(AB);
lengthBC = norm(BC);

% Calculate dot product and use law of cosines to find angle
dotProduct = dot(AB, BC);
cosAngle = dotProduct / (lengthAB * lengthBC);
angleRadians = acos(cosAngle);
angleDegrees = rad2deg(angleRadians);

% Plotting
figure;
plot([A(1), B(1)], [A(2), B(2)], 'b-o'); % Plot line AB (thigh)
hold on;
plot([B(1), C(1)], [B(2), C(2)], 'r-o'); % Plot line BC (shin)
plot(A(1), A(2), 'go', 'MarkerSize', 10); % Plot point A (hip)
plot(B(1), B(2), 'go', 'MarkerSize', 10); % Plot point B (knee)
plot(C(1), C(2), 'go', 'MarkerSize', 10); % Plot point C (ankle)
xlabel('X-axis');
ylabel('Y-axis');
title(['Angle between lines: ', num2str(angleDegrees), ' degrees']);
legend('Thigh (AB)', 'Shin (BC)', 'Hip (A)', 'Knee (B)', 'Ankle (C)');
grid on;
hold off;

disp(['The angle between the lines (thigh and shin) is: ', num2str(angleDegrees), ' degrees']);

