package de.kreativsoft.hello_cmake

import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.view.View
import android.widget.TextView
import de.kreativsoft.hello_cmake.databinding.ActivityMainBinding

class MainActivity : AppCompatActivity() {

    private lateinit var binding: ActivityMainBinding

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        binding = ActivityMainBinding.inflate(layoutInflater)
        setContentView(binding.root)

        // Example of a call to a native method
        binding.sampleText.text = stringFromJNI()

    }

    fun startPing(v: View) {
        pingJNI("8.8.8.8")
    }
    /**
     * A native method that is implemented by the 'hello_cmake' native library,
     * which is packaged with this application.
     */
    external fun stringFromJNI(): String
    external fun pingJNI(host: String?): Int


    companion object {
        // Used to load the 'hello_cmake' library on application startup.
        init {
            System.loadLibrary("hello_cmake")
        }
    }
}