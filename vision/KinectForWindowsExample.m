%% Using the Kinect for Windows V1 from Image Acquisition Toolbox
% This example shows how to obtain the data available from the Kinect(R) for
% Windows(R) V1 sensor using Image Acquisition Toolbox(TM):
%
% Copyright 2012-2017 The MathWorks, Inc.

%% Utility Functions
% In order the keep this example as simple as possible, some utility functions
% for working with the Kinect for Windows metadata have been created.
% These utility functions include the skeletalViewer function which accepts the
% skeleton data, color image and number of skeletons as inputs and displays the
% skeleton overlaid on the color image


%% See What Kinect for Windows Devices and Formats are Available
% The Kinect for Windows has two sensors, an color sensor and a depth
% sensor. To enable independent acquisition from each of these devices,
% they are treated as two independent devices in the Image
% Acquisition Toolbox. This means that separate VIDEOINPUT object needs to be created
% for each of the color and depth(IR) devices.

% The Kinect for Windows Sensor shows up as two separate devices in IMAQHWINFO.
hwInfo = imaqhwinfo('kinect');


%%
hwInfo.DeviceInfo(1);

%%
hwInfo.DeviceInfo(2);

%% Acquire Color and Depth Data
% In order to acquire synchronized color and depth data, we must use
% manual triggering instead of immediate triggering. The default immediate
% triggering suffers from a lag between streams while performing synchronized
% acquisition. This is due to the overhead in starting of streams sequentially.

% Create the VIDEOINPUT objects for the two streams
colorVid = videoinput('kinect',1);
%%
depthVid = videoinput('kinect',2);

%%
%

% Set the triggering mode to 'manual'
triggerconfig([colorVid depthVid],'manual');

%%
%
% Set the FramesPerTrigger property of the VIDEOINPUT objects to '100' to
% acquire 100 frames per trigger. In this example 100 frames are acquired to
% give the Kinect for Windows sensor sufficient time to start tracking a
% skeleton.
colorVid.FramesPerTrigger = 100;
depthVid.FramesPerTrigger = 100;

%%
%

% Start the color and depth device. This begins acquisition, but does not
% start logging of acquired data.
start([colorVid depthVid]);
%%
%

% Trigger the devices to start logging of data.
trigger([colorVid depthVid]);
%%
%

% Retrieve the acquired data
[colorFrameData, colorTimeData, colorMetaData] = getdata(colorVid);
[depthFrameData, depthTimeData, depthMetaData] = getdata(depthVid);
%%
%

% Stop the devices
stop([colorVid depthVid]);

%% Configure Skeletal Tracking
% The Kinect for Windows sensor provides different modes to track skeletons.
% These modes can be accessed and configured from the VIDEOSOURCE object of the
% depth device. Let's see how to enable skeleton tracking.

% Get the VIDEOSOURCE object from the depth device's VIDEOINPUT object.
depthSrc = getselectedsource(depthVid);

%%
% The properties on the depth source object that control the skeletal
% tracking features are TrackingMode, SkeletonToTrack and BodyPosture
% properties on the VIDEOSOURCE.
%
% TrackingMode controls whether or not skeletal tracking is enabled and,
% if it is enabled, whether all joints are tracked, 'Skeleton', or if just
% the hip position is tracked, 'Position'. Setting TrackingMode to 'off' (default)
% disables all tracking and reduces the CPU load.
%
% The 'BodyPosture' property determines how many joints are tracked.
% If 'BodyPosture' is set to 'Standing', twenty joints are tracked.  If it is
% set to 'Seated', then ten joints are tracked.
%
% The SkeletonToTrack property can be used to selectively track one or two
% skeletons using the 'SkeletonTrackingID'. The currently valid values for
% 'SkeletonTrackingID' are returned as a part of the metadata of the depth device.

%%
%

% Turn on skeletal tracking.
depthSrc.TrackingMode = 'Skeleton';

%% Access Skeletal Data
% The skeleton data that the Kinect for Windows produces is accessible from
% the depth device as a part of the metadata returned by GETDATA. The Kinect
% for Windows can track the position of up to six people in view and can
% actively track the joint locations of two of the six skeletons.
% It also supports two modes of tracking people based on whether
% they are standing or seated. In standing mode, the full 20 joint locations
% are tracked and returned; in seated mode the 10 upper body joints are returned.
% For more details on skeletal data, see the MATLAB documentation on Kinect for Windows adaptor.


%%
%

% Acquire 100 frames with tracking turned on.
% Remember to have a person in person in front of the
% Kinect for Windows to see valid tracking data.
colorVid.FramesPerTrigger = 100;
depthVid.FramesPerTrigger = 100;

%%
%
start([colorVid depthVid]);
trigger([colorVid depthVid]);

%%

% Retrieve the frames and check if any Skeletons are tracked
[frameDataColor] = getdata(colorVid);
[frameDataDepth, timeDataDepth, metaDataDepth] = getdata(depthVid);

%%
%

% View skeletal data from depth metadata
metaDataDepth

%%
% We randomly choose the 95th frame to visualize the image and skeleton
% data.

% Check for tracked skeletons from depth metadata
anyPositionsTracked = any(metaDataDepth(95).IsPositionTracked ~= 0)
anySkeletonsTracked = any(metaDataDepth(95).IsSkeletonTracked ~= 0)

%%
% The results above show that at least one skeleton is being tracked.
% If tracking is enabled but no IDs are specified with the TrackingID property,
% the Kinect for Windows software automatically chooses up to two skeletons
% to track. Use the IsSkeletonTracked metadata to determine which skeletons
% are being tracked.

% See which skeletons were tracked.
trackedSkeletons = find(metaDataDepth(95).IsSkeletonTracked)

%%
% Display skeleton's joint coordinates. Note that if the 'BodyPosture'
% property is set to 'Seated', the 'JointCoordinates' and 'JointIndices'
% will still have a length of 20, but indices 2-11(upper-body joints) alone will be populated.
jointCoordinates = metaDataDepth(95).JointWorldCoordinates(:, :, trackedSkeletons)
% Skeleton's joint indices with respect to the color image
jointIndices = metaDataDepth(95).JointImageIndices(:, :, trackedSkeletons)

%% Draw the Skeleton Over the Corresponding Color Image

% Pull out the 95th color frame
image = frameDataColor(:, :, :, 95);

% Find number of Skeletons tracked
nSkeleton = length(trackedSkeletons);

% Plot the skeleton
util_skeletonViewer(jointIndices, image, nSkeleton);
