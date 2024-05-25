package com.example.bloomie;

import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;
import android.view.WindowManager;
import android.os.Handler;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.graphics.Insets;
import androidx.core.view.ViewCompat;

import com.google.firebase.auth.FirebaseAuth;
import com.google.firebase.database.DataSnapshot;
import com.google.firebase.database.DatabaseError;
import com.google.firebase.database.DatabaseReference;
import com.google.firebase.database.FirebaseDatabase;
import com.google.firebase.database.ValueEventListener;

import androidx.core.view.WindowInsetsCompat;

import java.time.LocalDateTime;
import java.util.Calendar;



public class MainActivity extends AppCompatActivity {
    FirebaseAuth mAuth;
    private TextView id_temperatura;

    private TextView horaText;
    private TextView id_humedad;
    private TextView id_suelo;
    private DatabaseReference dht11;
    private DatabaseReference ground_humidity;

    private DatabaseReference name;


    private TextView userID;



    Button btn_logout;
    private DatabaseReference myRef;

    private  Handler handler = new Handler();
    private static final String TAG = "MainActivity"; // Define tu etiqueta de registro
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);
        setContentView(R.layout.activity_main);

        // Después de inflar el layout, puedes obtener la referencia al botón de logout
        btn_logout = findViewById(R.id.btn_logout);

        mAuth = FirebaseAuth.getInstance();
        // Write a message to the database
        horaText = findViewById(R.id.horaText);
        id_temperatura = findViewById(R.id.id_temperatura);
        id_humedad = findViewById(R.id.id_humedad);
        id_suelo = findViewById(R.id.id_suelo);
        userID = findViewById(R.id.idUser);
        FirebaseDatabase database = FirebaseDatabase.getInstance();
        dht11 = FirebaseDatabase.getInstance().getReference().child("dht11");
        ground_humidity = FirebaseDatabase.getInstance().getReference().child("ground_humidity");
        name = FirebaseDatabase.getInstance().getReference().child("Users");
        String id = mAuth.getCurrentUser().getUid();

        handler.postDelayed(updateDateTimeRunnable, 1000);

        int[] dateTime = getCurrentDateTime();
        int month = dateTime[0];
        int day = dateTime[1];
        int hour = dateTime[2];
        int minute = dateTime[3];
        int second = dateTime[4];
        btn_logout.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                // Aquí puedes agregar el código para cerrar sesión
                mAuth.signOut();
                finish();
                startActivity(new Intent( MainActivity.this, loginActivity.class));
            }
        });

        name.addValueEventListener(new ValueEventListener() {
            @Override
            public void onDataChange(DataSnapshot dataSnapshot) {
                // Este método se llama una vez con el valor inicial y nuevamente

                String  value = dataSnapshot.child(id).child("name").getValue(String.class);
                Log.d(TAG, "Value is: " + value);
                userID.setText(value+"");


            }

            @Override
            public void onCancelled(@NonNull DatabaseError error) {

            }
        });
        // Lee desde la base de datos
        dht11.addValueEventListener(new ValueEventListener() {
            @Override
            public void onDataChange(DataSnapshot dataSnapshot) {
                // Este método se llama una vez con el valor inicial y nuevamente

                Float  value = dataSnapshot.child("temperature").child("value").getValue(Float.class);
                Log.d(TAG, "Value is: " + value);
                id_temperatura.setText(value+"");

                Float  valueh = dataSnapshot.child("humidity").child("value").getValue(Float.class);
                Log.d(TAG, "Value is: " + valueh);
                id_humedad.setText(valueh+"");

            }

            @Override
            public void onCancelled(DatabaseError error) {
                // Error de lectura de datos
                Log.w(TAG, "Failed to read value.", error.toException());
            }
        });


        ground_humidity.addValueEventListener(new ValueEventListener() {
            @Override
            public void onDataChange(DataSnapshot dataSnapshot) {
                Float  values = dataSnapshot.child("ground_humidity").child("value").getValue(Float.class);
                Log.d(TAG, "Value is: " + values);
                id_suelo.setText(values+"");
            }

            @Override
            public void onCancelled(DatabaseError error) {
                // Error de lectura de datos
                Log.w(TAG, "Failed to read value.", error.toException());

            }
        });




    }

    private Runnable updateDateTimeRunnable = new Runnable() {
        @Override
        public void run() {
            int[] dateTime = getCurrentDateTime();
            int hour = dateTime[2];
            int minute = dateTime[3];
            int second = dateTime[4];

            // Formatear la hora actual en una cadena de texto
            String formattedDateTime = String.format("%02d:%02d:%02d", hour, minute, second);

            // Actualizar el TextView con la hora actual
            horaText.setText(formattedDateTime);

            // Programar la próxima actualización del TextView
            handler.postDelayed(this, 1000); // Actualizar cada segundo

        }
    };

    public static int[] getCurrentDateTime() {
        Calendar calendar = Calendar.getInstance();
        int month = calendar.get(Calendar.MONTH) + 1; // Sumamos 1 porque en Calendar, enero es 0
        int day = calendar.get(Calendar.DAY_OF_MONTH);
        int hour = calendar.get(Calendar.HOUR_OF_DAY);
        int minute = calendar.get(Calendar.MINUTE);
        int second = calendar.get(Calendar.SECOND);

        return new int[]{month, day, hour, minute, second};
    }
}