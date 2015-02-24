% Measure Performance of different
% Sorting ALgorithms
% * Serial Quicksort
% * Parallel Divide and Conquer
% * Peer parallelization

[NUM,TXT,RAW]=xlsread('performance.xlsx')

% Store the threads list
threads = NUM(2:7,1)
% Store the number of elements 
elements = NUM(1,2:end)
% Store the execution times of each algorithm
times = NUM(2:end,2:end)
serialQuickTimes = times(end,:)
divideConquerTimes = NUM(8:13,2:6)
peerParTimes = NUM(2:7,2:6)
 
% Execution time of quicksort plot
semilogx(elements,serialQuickTimes,'-o','MarkerSize',...
    6,'MarkerFaceColor','blue','color','blue','LineWidth',1.5),grid on
set(gca,'XTick',elements)
title('Execution time of Serial Quicksort for various array-sizes');
ylabel('Execution Time');
xlabel('Elements of array');

divideSpeedUpMatrix = zeros(6,5);
peerParSpeedUpMatrix = zeros(6,3);

for i=1:6
    for j=1:5
        divideSpeedUpMatrix(i,j) = serialQuickTimes(j)/divideConquerTimes(i,j);
        if(j<=3)
            peerParSpeedUpMatrix(i,j) = serialQuickTimes(j)/peerParTimes(i,j);
        end
    end
end

% Part to plot the speedups
% Plot the lines/times

% Create the colors
linecolors = lines(size(divideSpeedUpMatrix,2));

for i=1:size(divideSpeedUpMatrix,2)
    hold on;
    plot(threads(:),divideSpeedUpMatrix(:,i),'-o','MarkerFaceColor',linecolors(i,:),'color',linecolors(i,:));
    hold off;
end
grid on;
set(gca,'XTick',threads)
% set(gca,'XScale','log') 
title('Exec. Time SpeedUp of Divide & Conquer compared to QuickSort sorting algorithm');
ylabel('Execution Time Speedup X');
xlabel('Threads');
legend(num2str(elements(:)),'Location','NorthEast')

% Part to plot the speedups of peer parallelized sorting
% Plot the lines/times

% Create the colors
linecolors = lines(size(divideSpeedUpMatrix,2));

for i=1:size(divideSpeedUpMatrix,2)
    hold on;
    plot(threads(:),divideSpeedUpMatrix(:,i),'-o','MarkerFaceColor',linecolors(i,:),'color',linecolors(i,:));
    hold off;
end
grid on;
set(gca,'XTick',threads)
% set(gca,'XScale','log') 
title('Exec. Time SpeedUp of Divide & Conquer compared to QuickSort sorting algorithm');
ylabel('Execution Time Speedup X');
xlabel('Threads');
legend(num2str(elements(:)),'Location','NorthEast')

