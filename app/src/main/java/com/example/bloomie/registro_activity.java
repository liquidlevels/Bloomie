package com.example.bloomie;

import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Toast;

import androidx.activity.EdgeToEdge;
import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.graphics.Insets;
import androidx.core.view.ViewCompat;
import androidx.core.view.WindowInsetsCompat;

import com.google.android.gms.tasks.OnCompleteListener;
import com.google.android.gms.tasks.Task;
import com.google.firebase.auth.AuthResult;
import com.google.firebase.auth.FirebaseAuth;
import com.google.firebase.database.DatabaseReference;
import com.google.firebase.database.FirebaseDatabase;

import java.util.HashMap;
import java.util.Map;
import java.util.Objects;

public class registro_activity extends AppCompatActivity {
    private EditText reg_usuario;
    private  EditText reg_pass;
    private EditText reg_email;

    private Button btn_reg;

    String usuario = "";

    String email ="";
    String pass =   "";

    FirebaseAuth mAuth;
    DatabaseReference mDataBase;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        EdgeToEdge.enable(this);
        setContentView(R.layout.registro);
        mAuth = FirebaseAuth.getInstance();
        mDataBase = FirebaseDatabase.getInstance().getReference();

        reg_pass = findViewById(R.id.reg_pass);
        reg_usuario = findViewById(R.id.reg_usuario);
        reg_email = findViewById(R.id.reg_email);
        btn_reg = findViewById(R.id.btn_reg);

        btn_reg.setOnClickListener(new View.OnClickListener() {
        @Override
         public void onClick(View v) {

            usuario = reg_usuario.getText().toString().trim();
            email =   reg_email.getText().toString().trim();
            pass =    reg_pass.getText().toString().trim();

            if(!usuario.isEmpty() && !email.isEmpty() && !pass.isEmpty()){
                if(pass.length() >= 6){
                    registeUser();

                }else {
                    Toast.makeText(registro_activity.this, "La contraseña debe ser mayor a 6 caracteres",Toast.LENGTH_LONG).show();
                }
            }
         }
        });
    }

    /**
     * Se hace el registro del usuario  en firebase
     */
    private void registeUser(){
        mAuth.createUserWithEmailAndPassword(email,pass).addOnCompleteListener(new OnCompleteListener<AuthResult>() {
            @Override
            public void onComplete(@NonNull Task<AuthResult> task) {
                if(task.isSuccessful()){
                    //crea un mapa de objetso donde se guardan los datos que ingresa el usuario
                    Map<String, Object> map = new HashMap<>();
                    map.put("name", usuario);
                    map.put("email", email);
                    map.put("pass", pass);
                    //genera un id para el usuario que se esta registrando
                    String id = mAuth.getCurrentUser().getUid();
                    //se define la ruta donde se guardaran los datos del usuario
                    mDataBase.child("Users").child(id).setValue(map).addOnCompleteListener(new OnCompleteListener<Void>() {
                        @Override
                        public void onComplete(@NonNull Task<Void> task2) {
                            if(task2.isSuccessful()){
                                /*todo se realiza correctamente se cambia al layaut de login */
                                startActivity(new Intent(registro_activity.this, loginActivity.class));
                                finish();
                            }else {
                                //si ocurre un error manda un mensaje para avisar al usuario
                                Toast.makeText(registro_activity.this, "No se crearon lo datos correctamente", Toast.LENGTH_LONG).show();
                            }
                        }
                    });
                }else {
                    Toast.makeText(registro_activity.this, "No se pudo registrar", Toast.LENGTH_LONG).show();
                }
            }
        });
    }
}