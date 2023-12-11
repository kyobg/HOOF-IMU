% Define coordinates of body joints for a standing pose
head = [0, 6];
neck = [0, 5];
leftShoulder = [-1.5, 4];
rightShoulder = [1.5, 4];
leftElbow = [-2, 2.5];
rightElbow = [2, 2.5];
leftHand = [-2.5, 1];
rightHand = [2.5, 1];
torsoTop = [0, 5];
torsoBottom = [0, 2];
leftHip = [-1, 2];
rightHip = [1, 2.5];
leftKnee = [-1, 0];
rightKnee = [1, 0];
leftFoot = [-1, -2];
rightFoot = [1, -2.5];

% Connect body joints to form skeleton lines
lines = [
    head, neck;
    neck, leftShoulder;
    neck, rightShoulder;
    leftShoulder, leftElbow;
    rightShoulder, rightElbow;
    leftElbow, leftHand;
    rightElbow, rightHand;
    neck, torsoTop;
    torsoTop, torsoBottom;
    torsoBottom, leftHip;
    torsoBottom, rightHip;
    leftHip, leftKnee;
    rightHip, rightKnee;
    leftKnee, leftFoot;
    rightKnee, rightFoot
];

% Plotting skeleton
figure;
for i = 1:size(lines, 1)
    plot([lines(i, 1), lines(i, 3)], [lines(i, 2), lines(i, 4)], 'b', 'LineWidth', 2);
    hold on;
end

% Plot body joints
plot(head(1), head(2), 'ro', 'MarkerSize', 8);
plot(neck(1), neck(2), 'ro', 'MarkerSize', 8);
plot(leftShoulder(1), leftShoulder(2), 'ro', 'MarkerSize', 8);
plot(rightShoulder(1), rightShoulder(2), 'ro', 'MarkerSize', 8);
plot(leftElbow(1), leftElbow(2), 'ro', 'MarkerSize', 8);
plot(rightElbow(1), rightElbow(2), 'ro', 'MarkerSize', 8);
plot(leftHand(1), leftHand(2), 'ro', 'MarkerSize', 8);
plot(rightHand(1), rightHand(2), 'ro', 'MarkerSize', 8);
plot(torsoTop(1), torsoTop(2), 'ro', 'MarkerSize', 8);
plot(torsoBottom(1), torsoBottom(2), 'ro', 'MarkerSize', 8);
plot(leftHip(1), leftHip(2), 'ro', 'MarkerSize', 8);
plot(rightHip(1), rightHip(2), 'ro', 'MarkerSize', 8);
plot(leftKnee(1), leftKnee(2), 'ro', 'MarkerSize', 8);
plot(rightKnee(1), rightKnee(2), 'ro', 'MarkerSize', 8);
plot(leftFoot(1), leftFoot(2), 'ro', 'MarkerSize', 8);
plot(rightFoot(1), rightFoot(2), 'ro', 'MarkerSize', 8);

xlabel('X-axis');
ylabel('Y-axis');
title('Standing Human Skeleton');
axis equal;
grid on;
hold off;
