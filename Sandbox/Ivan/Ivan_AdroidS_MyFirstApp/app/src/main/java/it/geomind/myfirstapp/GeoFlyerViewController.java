package it.geomind.myfirstapp;

import android.content.Intent;
import android.os.Bundle;
import android.support.v7.app.ActionBarActivity;
import android.view.View;

/**

 */
public class GeoFlyerViewController extends ActionBarActivity implements MyAlertDialogFragment.MyAlertDialogListener
{
    public GeoFlyerViewController()
    {
        mGeoFlyerController = CreateGeoFlyerController();
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_geo_flyer_view_controller);
    }

    @Override
    protected void finalize() throws Throwable
    {
        ReleaseResources();
        super.finalize();
    }

    @Override
    protected void onDestroy()
    {
        ReleaseResources();
        super.onDestroy();
    }

    public void ReleaseResources()
    {
        if( mGeoFlyerController != 0 )
        {
            DestroyGeoFlyerController( mGeoFlyerController );
            mGeoFlyerController = 0;
        }
    }

    public void dialogFromCpp(View view) {
        if( mGeoFlyerController != 0 )
            DebugGeoFlyerControllerShowDialog( mGeoFlyerController );
    }

    public void messageBoxAndButtons( String inTitle, String inMsg, String[] inButtonStrings )
    {
        MyAlertDialogFragment dialog = new MyAlertDialogFragment( inTitle, inMsg, inButtonStrings, this );
        dialog.show( getSupportFragmentManager(), "GeoFlyerViewController_MyAlertDialogFragment" );
     }

    public void onFinishDialog( int inButtonIndex )
    {
        if( mGeoFlyerController != 0 )
            OnMessageBoxResult( mGeoFlyerController, inButtonIndex );
    }


    private long mGeoFlyerController;

    private native long CreateGeoFlyerController();
    private native void DestroyGeoFlyerController( long inGeoFlyerController );
    private native void DebugGeoFlyerControllerShowDialog( long inGeoFlyerController );
    private native void	OnMessageBoxResult( long inGeoFlyerController, int inChoice ); // from viewcontroller to this (GeoFlyerCOntroller will call the inCallback of the inDelegate)
}
