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
import com.google.android.gms.tasks.OnFailureListener;
import com.google.android.gms.tasks.Task;
import com.google.firebase.auth.AuthResult;
import com.google.firebase.auth.FirebaseAuth;
import com.google.firebase.auth.FirebaseAuthException;
import com.google.firebase.ktx.Firebase;

import java.util.Stack;

public class loginActivity extends AppCompatActivity {

    Button btn_entrar,btn_registrar;
    EditText id_email, id_password;
    FirebaseAuth mAuth;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        EdgeToEdge.enable(this);
        setContentView(R.layout.login);
        mAuth = FirebaseAuth.getInstance();

        id_email = findViewById(R.id.id_email);
        id_password = findViewById(R.id.id_password);
        btn_entrar = findViewById(R.id.btn_entrar);
        btn_registrar = findViewById(R.id.btn_regristrar);

        btn_entrar.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                String emailUser = id_email.getText().toString().trim();
                String passUser = id_password.getText().toString().trim();

                if(emailUser.isEmpty() && passUser.isEmpty()){
                    Toast.makeText(loginActivity.this, "Ingresar datos", Toast.LENGTH_LONG).show();

                }else {
                    loginUser(emailUser, passUser);
                }
            }
        });
    }

    private void loginUser(String emailUser, String passUser){
        mAuth.signInWithEmailAndPassword(emailUser,passUser).addOnCompleteListener(new OnCompleteListener<AuthResult>() {
            @Override
            public void onComplete(@NonNull Task<AuthResult> task) {
                if (task.isSuccessful()){
                    finish();
                    startActivity(new Intent(loginActivity.this,MainActivity.class));
                    Toast.makeText(loginActivity.this,"Bienvenido", Toast.LENGTH_LONG).show();
                }else{
                    Toast.makeText(loginActivity.this,"ERROR", Toast.LENGTH_LONG).show();
                }
            }
        }).addOnFailureListener(new OnFailureListener() {
            @Override
            public void onFailure(@NonNull Exception e) {
                Toast.makeText(loginActivity.this,"Error al iniciar Sesion",Toast.LENGTH_LONG).show();
            }
        });
    }


}