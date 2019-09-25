function client(port)
%   provides a menu for accessing PIC32 motor control functions
%
%   client(port)
%
%   Input Arguments:
%       port - the name of the com port.  This should be the same as what
%               you use in screen or putty in quotes ' '
%
%   Example:
%       client('/dev/ttyUSB0') (Linux/Mac)
%       client('COM3') (PC)
%
%   For convenience, you may want to change this so that the port is hardcoded.
   
% Opening COM connection
if ~isempty(instrfind)
    fclose(instrfind);
    delete(instrfind);
end

fprintf('Opening port %s....\n',port);

% settings for opening the serial port. baud rate 230400, hardware flow control
% wait up to 120 seconds for data before timing out
mySerial = serial(port, 'BaudRate', 230400, 'FlowControl', 'hardware','Timeout',120); 
% opens serial connection
fopen(mySerial);
% closes serial port when function exits
clean = onCleanup(@()fclose(mySerial));                                 

has_quit = false;
% menu loop
while ~has_quit
    fprintf('\nPIC32 MOTOR DRIVER INTERFACE\n\n');
    % display the menu options; this list will grow
    fprintf('     a: Read current sensor (ADC counts)    b: Read current sensor (mA)\n');
    fprintf('     c: Read encoder (counts)               d: Read encoder (deg)\n');
    fprintf('     e: Reset encoder                       f: Set PWM (-100 to 100)\n');
    fprintf('     g: Set current gains                   h: Get current gains\n');
    fprintf('     i: Set position gains                  j: Get position gains\n');
    fprintf('     k: Test current control                l: Go to angle (deg)\n');
    fprintf('     m: Load step trajectory                n: Load cubic trajectory\n');
    fprintf('     o: Execute trajectory                  p: Unpower the motor\n');
    fprintf('     q: Quit client                         r: Get mode\n');
    % read the user's choice
    selection = input('\nENTER COMMAND: ', 's');
     
    % send the command to the PIC32
    fprintf(mySerial,'%c\n',selection);
    
    % take the appropriate action
    switch selection
        case 'a'
            ADC = fscanf(mySerial,'%d');
            fprintf('\nADC counts = %d\n',ADC);
            
        case 'b'
            mA = fscanf(mySerial,'%d');
            fprintf('\nCurrent (in mA) = %d\n',mA);
            
        case 'c'
            counts = fscanf(mySerial,'%d');
            fprintf('\nThe motor angle is %d counts.\n',counts);
            
        case 'd'                         % example operation
            countsDeg = fscanf(mySerial,'%d');   % get the incremented number back
            fprintf('\nThe motor angle is %d deg.\n',countsDeg);     % print it to the screen
            
        case 'e'
            counts = fscanf(mySerial,'%d');
            fprintf('\nThe motor angle is %d counts.\n',counts);
            
        case 'f'
            DC = input('\nEnter Duty cycle [-100 to 100] = ');
            fprintf(mySerial,'%d\n',DC);
            
        case 'g'
            fprintf('\nEnter Current Gains:\n');
            
            Kp = input('\nEnter Kp (Proportional Gain) = ');
            fprintf(mySerial,'%d\n',Kp);
            
            Ki = input('Enter Ki (Integral Gain)     = ');
            fprintf(mySerial,'%d\n',Ki);
            
        case 'h'
            fprintf('\nCurrent Gains:\n');
            
            Pg = fscanf(mySerial,'%d');
            fprintf('\nKp (Proportional Gain) = %d\n',Pg);
            
            Ig = fscanf(mySerial,'%f');
            fprintf('Ki (Integral Gain)     = %d\n',Ig);
            
        case 'i'
            fprintf('\nEnter Position Gains:\n');
            
            Kp_pos = input('\nEnter Kp (Proportional Gain) = ');
            fprintf(mySerial,'%f\n',Kp_pos);
            
            Ki_pos = input('Enter Ki (Integral Gain)     = ');
            fprintf(mySerial,'%d\n',Ki_pos);
            
            Kd_pos = input('Enter Kd (Derivative Gain)   = ');
            fprintf(mySerial,'%d\n',Kd_pos);
            
        case 'j'
            fprintf('\nPosition Gains:\n');
            
            Pg_pos = fscanf(mySerial,'%d');
            fprintf('Kp_position (Proportional Gain) = %d\n',Pg_pos);
            
            Ig_pos = fscanf(mySerial,'%d');
            fprintf('Ki (Integral Gain)     = %d\n',Ig_pos);
            
            Id_pos = fscanf(mySerial,'%d');
            fprintf('Kd (Derivative Gain)   = %d\n',Id_pos); 
            
        case 'k'
            data = read_plot_matrix(mySerial);
            
        case 'l'
            angle = input('\nEnter angle (in degrees) = ');
            fprintf(mySerial,'%f\n',angle);
            
        case 'm'
            A = input("Enter Step trajectory: ");
            refTraj = genRef(A,'step');
            fprintf(mySerial,'%d\n',length(refTraj));
            for i = 1:length(refTraj)
                fprintf(mySerial,'%d\n',refTraj(i));
            end
            
        case 'n'
            B = input("Enter Cubic trajectory: ");
            refTraj = genRef(B,'cubic');
            fprintf(mySerial,'%d\n',length(refTraj));
            for i = 1:length(refTraj)
                fprintf(mySerial,'%d\n',refTraj(i));
            end
            
        case 'o'
            data = read_plot_matrix(mySerial);
            
        case 'p'
            
        case 'q'
            has_quit = true;             % exit client
            
        case 'r'
            mode = fscanf(mySerial,'%c');
            fprintf('%s',mode);
            
        otherwise
            fprintf('Invalid Selection %c\n', selection);
    end
end

end