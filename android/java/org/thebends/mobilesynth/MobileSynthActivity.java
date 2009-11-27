package org.thebends.mobilesynth;

import android.app.Activity;
import android.os.Bundle;
import org.thebends.synth.Oscillator;
import org.thebends.synth.FixedParameter;

import java.util.logging.Logger;

public class MobileSynthActivity extends Activity {
  private static Logger LOG = Logger.getLogger(
      MobileSynthActivity.class.getSimpleName());

  private Oscillator osc;
  private SynthTrack t;

  /** Called when the activity is first created. */
  @Override
  public void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    LOG.info("Creating synth activity");
    // TODO(allen): Layout the keyboard
    setContentView(R.layout.main);

    osc = new Oscillator(FixedParameter.get(1000), 44100);
    osc.setWaveForm(Oscillator.WaveForm.SAWTOOTH);
  }

  private void play() {
    t = new SynthTrack(osc);
  }

  @Override
  protected void onPause() {
    super.onPause();
    LOG.info("Pausing synth activity");
    t.shutdown();
    t = null;
  }

  @Override
  protected void onResume() {
    super.onResume();
    LOG.info("Resuming synth activity");
    play();
  }
}
