package timo.home.vulkanFFT;

import androidx.appcompat.app.AppCompatActivity;
import android.os.Bundle;
import android.content.res.AssetManager;

//Button
import android.widget.Button;
import android.view.View;
import android.view.View.OnClickListener;
//Debug
import android.util.Log;

public class VulkanFFT extends AppCompatActivity  {
	//Load native library and define the calls
	static {System.loadLibrary("vulkanFFT");} //Load jni vulkanFFT, remember to clean up...
	static native boolean prepVulkan(AssetManager manager, int signalLength);
	static native void compute(float[] arrIn);
    static native void delete();
    int vulkanReady = 1;
	

	 private static final String TAG = VulkanFFT.class.getName();
	Button computeButton;
	
	
	
	
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);
		//Get references to GUI buttons
		computeButton = (Button) findViewById(R.id.computeButton);
     
		
		prepVulkan(this.getAssets(), 1<<5);
		//Bind GUI listeners
		computeButton.setOnClickListener( new View.OnClickListener() {
			public void onClick(View v) {
					//Shutdown services
					call_compute();
				 }
			}
		);
    }

	protected void call_compute(){
		Log.e(TAG,"Compute pressed");
		compute(new float[]{0f,1f,2f});
	}
	
	/*Power saving*/
     protected void onResume() {
		//Restart vulkanFFT
		if (vulkanReady == 0){
			Log.e(TAG,"onResume");
			prepVulkan(this.getAssets(), 1<<5);
			vulkanReady = 1;
		}
		super.onResume();
     }

     protected void onPause() {
		 Log.e(TAG,"onPause");
		delete();	//Delete the vulkanFFT
		vulkanReady = 0;
		finish();	//Remove this, need for debugging
		super.onPause();
     }
	
	protected void onDestroy(){
      super.onDestroy();
	}
	
}
