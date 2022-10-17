import java.util.ArrayList;
import java.util.LinkedList;

public class Frame {
    
    public static class FrameCell {
	public int process = -1;
	public int page  = -1;
}
	int frames;
	int FIFO = 0;
	LinkedList<Integer> LRUData = new LinkedList<Integer>();
	Integer[] ClockData;
	Integer clockPointer = 0;
	
	Frame(int frames){
		this.frames = frames;
		for (int i=0; i<frames; i++) LRUData.addLast(i);
		ClockData =  new Integer[frames];
		for (int i=0; i<frames; i++) ClockData[i] = 0;
	}
	
	public int getNextFifoFrame() {
		Integer next = FIFO;
		FIFO = (FIFO+1)%frames;
		return next;
	}
	
	public int getNextLRU() {
		int next = LRUData.removeFirst();
		LRUData.addLast(next);
		return next;
	}
	
	public int getNextClk() {
		Integer returnVal = null;
		while(ClockData[clockPointer] != 0) {
			ClockData[clockPointer] = 0;
			clockPointer = (clockPointer +1) % frames;
		}
		returnVal = clockPointer;
		ClockData[clockPointer] = 1;
		clockPointer = (clockPointer +1) % frames;
		return returnVal;
	}
	
	public int nextOptimalRoundRobin(int[] pagePos, ArrayList<Integer>[] pageRequests, FrameCell[] curFrames) {
		int[] nextRequest = new int[curFrames.length];
		for (int i=0; i<curFrames.length; i++) {
			nextRequest[i] = Integer.MAX_VALUE;
			int process = curFrames[i].process;
			if (process<0) return i;
			for (int j=pagePos[process]; j<pageRequests[process].size();j++) {
				if (pageRequests[process].get(j)==curFrames[i].page) {
					nextRequest[i] = j-pagePos[process];
					break;
				}
			}
		}
		
		//find index of max
		int max = -1;
		int maxFrame = 0;
		for (int i=0; i<curFrames.length; i++) {
			if (nextRequest[i] > max) {
				max = nextRequest[i];
				maxFrame = i;
			}
		}
		return maxFrame;
	}
	
	public int nextOptimalSequential(int pagePos, int process, ArrayList<Integer>[] pageRequests, FrameCell[] curFrames) {
		int[] nextRequest = new int[curFrames.length];
		for (int i=0; i<curFrames.length; i++) {
			if (curFrames[i].process != process) return i;
			nextRequest[i] = Integer.MAX_VALUE;
			for (int j=pagePos; j<pageRequests[process].size();j++) {
				if (pageRequests[process].get(j)==curFrames[i].page) {
					nextRequest[i] = j-pagePos;
					break;
				}
			}
		}
		//find index of max
		int max = -1;
		int maxFrame = 0;
		for (int i=0; i<curFrames.length; i++) {
			if (nextRequest[i] > max) {
				max = nextRequest[i];
				maxFrame = i;
			}
		}
		return maxFrame;
	}
	
	public void updateLRU(int i) {
		LRUData.removeFirstOccurrence(i);
		LRUData.addLast(i);
	}
	
	public void updateClk(int i) {
		ClockData[i] = ClockData[i]+1;
	}
}
