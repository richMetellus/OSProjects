
import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.util.ArrayList;
import java.util.Scanner;

public class PageReplacement {

    static int frame_qnt;  // the buffer size

    public static void main(String[] args) {
        Scanner input = new Scanner(System.in);
        System.out.print("Enter the Buffer Size: ");
        frame_qnt = input.nextInt();
        input.nextLine();
        System.out.println("Buffer Size: " + Integer.toString(frame_qnt) + "\n\n");
        Frame.FrameCell[] FIFOframes = new Frame.FrameCell[frame_qnt];
        Frame.FrameCell[] LRUframes = new Frame.FrameCell[frame_qnt];
        Frame.FrameCell[] optimalFrames = new Frame.FrameCell[frame_qnt];
        Frame.FrameCell[] clockFrames = new Frame.FrameCell[frame_qnt];
        for (int i = 0; i < frame_qnt; i++) {
            FIFOframes[i] = new Frame.FrameCell();
            LRUframes[i] = new Frame.FrameCell();
            optimalFrames[i] = new Frame.FrameCell();
            clockFrames[i] = new Frame.FrameCell();
        }
        Integer FIFO_Faults = 0, LRU_Faults = 0, clockFaults = 0, optimalFaults = 0;
        ArrayList<Integer>[] pageRequests = getInputFileData();

        //round robin
        Frame next = new Frame(frame_qnt);
        Boolean done = false;
        int curProcess = 0, numRequests = 0;
        int[] pagePos = new int[pageRequests.length];
        for (int i = 0; i < pagePos.length; i++) {
            pagePos[i] = 0;
        }
        while (!done) {
            int temp;
            //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~LRU ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
            temp = contains(LRUframes, curProcess, pageRequests[curProcess].get(pagePos[curProcess]));
            if (temp == -1) {
                int newFrame = next.getNextLRU();
                LRUframes[newFrame].process = curProcess;
                LRUframes[newFrame].page = pageRequests[curProcess].get(pagePos[curProcess]);
                LRU_Faults++;
            } else {
                next.updateLRU(temp);
            }
            //~~~~~~~~~~~~~~~~~~~~~~~~~~~FIFO~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
            temp = contains(FIFOframes, curProcess, pageRequests[curProcess].get(pagePos[curProcess]));
            if (temp == -1) {
                int newFrame = next.getNextFifoFrame();
                FIFOframes[newFrame].process = curProcess;
                FIFOframes[newFrame].page = pageRequests[curProcess].get(pagePos[curProcess]);
                FIFO_Faults++;
            } else {

            }
            
            //~~~~~~~~~~~~~~~~~~~~~~~~~~~CLOCK~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
            temp = contains(clockFrames, curProcess, pageRequests[curProcess].get(pagePos[curProcess]));
            if (temp == -1) {
                int newFrame = next.getNextClk();
                clockFrames[newFrame].process = curProcess;
                clockFrames[newFrame].page = pageRequests[curProcess].get(pagePos[curProcess]);
                clockFaults++;
            } else {
                next.updateClk(temp);
            }
            //~~~~~~~~~~~~~~~~~~~~~~~~~~~Optimal~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
            temp = contains(optimalFrames, curProcess, pageRequests[curProcess].get(pagePos[curProcess]));
            if (temp == -1) {
                int newFrame = next.nextOptimalRoundRobin(pagePos, pageRequests, optimalFrames);
                optimalFrames[newFrame].process = curProcess;
                optimalFrames[newFrame].page = pageRequests[curProcess].get(pagePos[curProcess]);
                optimalFaults++;
            }
            
            int oldProcess = curProcess;
            pagePos[curProcess]++;
            do {
                curProcess = (curProcess + 1) % pageRequests.length;
                if (curProcess == oldProcess) {
                    if (pagePos[curProcess] >= pageRequests[curProcess].size()) {
                        done = true;
                    }
                    break;
                }
            } while (pagePos[curProcess] >= pageRequests[curProcess].size());
            numRequests++;
        }
        
        Integer rr_Fifo_fault = FIFO_Faults;
        Integer rr_lru_fault = LRU_Faults;
        Integer rr_clk_fault = clockFaults;
        Integer rr_opt_fault = optimalFaults;
        

        FIFO_Faults = 0;
        LRU_Faults = 0;
        clockFaults = 0;
        optimalFaults = 0;

        for (int i = 0; i < frame_qnt; i++) {
            FIFOframes[i] = new Frame.FrameCell();
            LRUframes[i] = new Frame.FrameCell();
            optimalFrames[i] = new Frame.FrameCell();
            clockFrames[i] = new Frame.FrameCell();
        }

        next = new Frame(frame_qnt);
        for (int i = 0; i < pagePos.length; i++) {
            pagePos[i] = 0;
        }
        for (curProcess = 0; curProcess < pageRequests.length; curProcess++) {
            for (pagePos[curProcess] = 0; pagePos[curProcess] < pageRequests[curProcess].size(); pagePos[curProcess]++) {
                int temp;
                //~~~~~~~~~~~~~~~~~~~~~~~~~~~FIFO~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                temp = contains(FIFOframes, curProcess, pageRequests[curProcess].get(pagePos[curProcess]));
                if (temp == -1) {
                    int newFrame = next.getNextFifoFrame();
                    FIFOframes[newFrame].process = curProcess;
                    FIFOframes[newFrame].page = pageRequests[curProcess].get(pagePos[curProcess]);
                    FIFO_Faults++;
                }
                //~~~~~~~~~~~~~~~~~~~~~~~~~~~LRU~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                temp = contains(LRUframes, curProcess, pageRequests[curProcess].get(pagePos[curProcess]));
                if (temp == -1) {
                    int newFrame = next.getNextLRU();
                    LRUframes[newFrame].process = curProcess;
                    LRUframes[newFrame].page = pageRequests[curProcess].get(pagePos[curProcess]);
                    LRU_Faults++;
                } else {
                    next.updateLRU(temp);
                }
                //~~~~~~~~~~~~~~~~~~~~~~~~~~~CLOCK~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                temp = contains(clockFrames, curProcess, pageRequests[curProcess].get(pagePos[curProcess]));
                if (temp == -1) {
                    int newFrame = next.getNextClk();
                    clockFrames[newFrame].process = curProcess;
                    clockFrames[newFrame].page = pageRequests[curProcess].get(pagePos[curProcess]);
                    clockFaults++;
                } else {
                    next.updateClk(temp);
                }
                //~~~~~~~~~~~~~~~~~~~~~~~~~~~Optimal~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                temp = contains(optimalFrames, curProcess, pageRequests[curProcess].get(pagePos[curProcess]));
                if (temp == -1) {
                    int newFrame = next.nextOptimalSequential(pagePos[curProcess], curProcess, pageRequests, optimalFrames);
                    optimalFrames[newFrame].process = curProcess;
                    optimalFrames[newFrame].page = pageRequests[curProcess].get(pagePos[curProcess]);
                    optimalFaults++;
                } else {

                }
            }
        }
        System.out.println("\n\nConsecutive:");
        System.out.printf("----------------------\n");
        System.out.printf("%10s         %2.2f%%\n", "FIFO Page Fault", (float) FIFO_Faults * 100 / (float) numRequests);
        System.out.printf("%12s          %2.2f%%\n", "LRU Page Fault", (float) LRU_Faults * 100 / (float) numRequests);
        System.out.printf("%10s        %2.2f%%\n", "Clock Page Fault", (float) clockFaults * 100 / (float) numRequests);
        System.out.printf("%10s      %2.2f%%\n", "Optimal Page Fault", (float) optimalFaults * 100 / (float) numRequests);

        
        System.out.println("\nRound Robin:");
        System.out.println("----------------------");
        System.out.printf("%10s         %2.2f%%\n", "FIFO Page Fault", (float) rr_Fifo_fault * 100 / (float) numRequests);
        System.out.printf("%10s          %2.2f%%\n", "LRU Page Fault", (float) rr_lru_fault * 100 / (float) numRequests);
        System.out.printf("%10s        %2.2f%%\n", "Clock Page Fault", (float) rr_clk_fault * 100 / (float) numRequests);
        System.out.printf("%10s      %2.2f%%\n", "Optimal Page Fault", (float) rr_opt_fault * 100 / (float) numRequests);
    }

    public static int contains(Frame.FrameCell[] frames, int process, int pageNum) {
        for (int i = 0; i < frames.length; i++) {
            if (frames[i].page == pageNum && frames[i].process == process) {
                return i;
            }
        }
        return -1;
    }
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Utility ~~~~~~~~~~~~~~~~~~~~~~~~~
    public static ArrayList<Integer>[] getInputFileData() {
        BufferedReader reader = null;
        File folder = new File("./PageRequest");
        File[] files = folder.listFiles();
        if (files == null) {
            System.out.println("unable to locate PageRequest folder./"
                    + "make sure to include in the same directory'");
            System.exit(0);
        }
        @SuppressWarnings("unchecked")
        ArrayList<Integer>[] pageRequests = new ArrayList[files.length];
        for (int i = 0; i < files.length; i++) {
            pageRequests[i] = new ArrayList<Integer>();
        }
        for (int i = 0; i < files.length; i++) {
            try {
                reader = new BufferedReader(new FileReader(files[i]));
                String line;
                while ((line = reader.readLine()) != null) {
                    String[] requests = line.split(",");
                    for (int j = 0; j < requests.length; j++) {
                        pageRequests[i].add(Integer.parseInt(requests[j]));
                    }
                }
            } catch (Exception e) {
                System.out.println("Error parsing input files");
                e.printStackTrace();
            }
        }
        return pageRequests;
    }

}
