% Read temperature data from serial port and plot
close all; clear all; clc
fclose(INSTRFIND);
%delete(obj);
figure(1); clf; 



obj = serial('/dev/tty.usbmodem1d11');
fopen(obj);

%while obj.BytesAvailable==0
%    d=0;
%end

Array =[];
d=0;
while obj.BytesAvailable >= 0
    d=fscanf(obj, '%f %f');
    if d(1)~= 0
        %Array=[Array; d]
        d
        plot(d(2),d(1),'b.')
        hold on
    end
end

fclose(obj);

title('Reflow Run')
xlabel('Time (s)'); ylabel('Temperature (C)')

fclose(obj);
delete(obj);
hold off
