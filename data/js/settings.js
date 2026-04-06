document.querySelectorAll('.pass-element, .pin-element').forEach(element => {
    const input = element.querySelector('input');
    const toggle = element.querySelector('.toggle-visib');

    if (!toggle) {
        return;
    }

    const eyeClosed = toggle.querySelector('.eye-closed');
    const eyeOpen = toggle.querySelector('.eye-open');

    toggle.addEventListener('click', () => {
        if (input.type === 'password') {
            input.type = 'text';
            eyeClosed.style.display = 'none';
            eyeOpen.style.display = 'block';
        } else {
            input.type = 'password';
            eyeClosed.style.display = 'block';
            eyeOpen.style.display = 'none';
        }
    });

    input.addEventListener('input', () => {
        if (input.type === 'text') {
            input.type = 'password';
            eyeClosed.style.display = 'block';
            eyeOpen.style.display = 'none';
        }
    });
});

const changePasswordForm = document.getElementById('change-password-form');

if (changePasswordForm) {
    changePasswordForm.addEventListener('submit', (e) => {
        e.preventDefault();
        
        const currentPassword = document.getElementById('current-password').value;
        const newPassword = document.getElementById('new-password').value;
        const confirmPassword = document.getElementById('confirm-password').value;
        
        // Asigură-te că aceste elemente există în HTML-ul tău (settings.html nu le avea)
        const currentPasswordError = document.querySelector('.current-password-error');
        const newPasswordError = document.querySelector('.new-password-error');
        const confirmNewPasswordError = document.querySelector('.confirm-new-password-error');

        // Resetăm mesajele de eroare la fiecare submit
        if (currentPasswordError) currentPasswordError.style.display = 'none';
        if (newPasswordError) newPasswordError.style.display = 'none';
        if (confirmNewPasswordError) confirmNewPasswordError.style.display = 'none';


        if(newPassword !== confirmPassword) {
            if (confirmNewPasswordError) {
                confirmNewPasswordError.textContent = 'New password and confirmation do not match.';
                confirmNewPasswordError.style.display = 'block';
            } else {
                alert('New password and confirmation do not match.');
            }
            return;
        }

        if(newPassword.length <= 3) {
            if (newPasswordError) {
                newPasswordError.textContent = 'New password must be more than 3 characters long.';
                newPasswordError.style.display = 'block';
            } else {
                 alert('New password must be more than 3 characters long.');
            }
            return;   
        }

        fetch('/change-password', {
            method: 'POST',
            headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
            body: `current-password=${encodeURIComponent(currentPassword)}&new-password=${encodeURIComponent(newPassword)}`
        })
        .then(response => {
            if (response.ok) {
                alert('Password changed successfully!');
                changePasswordForm.reset();
            } else if(response.status === 403) {
                 if (currentPasswordError) {
                    currentPasswordError.textContent = 'Current password is incorrect.';
                    currentPasswordError.style.display = 'block';
                 } else {
                     alert('Current password is incorrect.');
                 }
            } else {            
                alert('An error occurred while changing the password. Please try again later.');
            }
        })
        .catch(error => {
            console.error('Error changing password:', error);
            alert('A network error occurred.');
        });
    }); 
}


