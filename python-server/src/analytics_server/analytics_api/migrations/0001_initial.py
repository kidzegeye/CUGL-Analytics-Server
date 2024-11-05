# Generated by Django 4.2.16 on 2024-11-05 00:07

from django.db import migrations, models
import django.db.models.deletion
import django.utils.timezone


class Migration(migrations.Migration):

    initial = True

    dependencies = [
    ]

    operations = [
        migrations.CreateModel(
            name='Game',
            fields=[
                ('id', models.BigAutoField(auto_created=True, primary_key=True, serialize=False, verbose_name='ID')),
                ('version_number', models.TextField()),
            ],
        ),
        migrations.CreateModel(
            name='GameMetaData',
            fields=[
                ('id', models.BigAutoField(auto_created=True, primary_key=True, serialize=False, verbose_name='ID')),
                ('package_name', models.TextField(unique=True)),
                ('game_name', models.TextField()),
            ],
        ),
        migrations.CreateModel(
            name='Session',
            fields=[
                ('id', models.BigAutoField(auto_created=True, primary_key=True, serialize=False, verbose_name='ID')),
                ('started_at', models.DateTimeField(default=django.utils.timezone.now)),
                ('ended_at', models.DateTimeField()),
                ('ended', models.BooleanField()),
            ],
        ),
        migrations.CreateModel(
            name='User',
            fields=[
                ('id', models.BigAutoField(auto_created=True, primary_key=True, serialize=False, verbose_name='ID')),
                ('vendor_id', models.TextField()),
                ('platform', models.TextField(choices=[('iOS', 'Ios'), ('Android', 'Android'), ('Other', 'Other')])),
                ('game', models.ForeignKey(on_delete=django.db.models.deletion.CASCADE, to='analytics_api.game')),
            ],
        ),
        migrations.CreateModel(
            name='TaskAttempt',
            fields=[
                ('id', models.BigAutoField(auto_created=True, primary_key=True, serialize=False, verbose_name='ID')),
                ('started_at', models.DateTimeField(default=django.utils.timezone.now)),
                ('ended_at', models.DateTimeField()),
                ('status', models.TextField(choices=[('Succeeded', 'Succeeded'), ('Failed', 'Failed'), ('Pending', 'Pending'), ('NotStarted', 'Notstarted'), ('Preempted', 'Preemped')])),
                ('num_failures', models.IntegerField(default=0)),
                ('statistics', models.JSONField(default=dict)),
                ('session', models.ForeignKey(on_delete=django.db.models.deletion.CASCADE, to='analytics_api.session')),
            ],
        ),
        migrations.CreateModel(
            name='Task',
            fields=[
                ('id', models.BigAutoField(auto_created=True, primary_key=True, serialize=False, verbose_name='ID')),
                ('name', models.TextField()),
                ('user', models.ForeignKey(on_delete=django.db.models.deletion.CASCADE, to='analytics_api.user')),
            ],
        ),
        migrations.AddField(
            model_name='session',
            name='user',
            field=models.ForeignKey(on_delete=django.db.models.deletion.CASCADE, to='analytics_api.user'),
        ),
        migrations.AddField(
            model_name='game',
            name='gamemetadata',
            field=models.ForeignKey(on_delete=django.db.models.deletion.CASCADE, to='analytics_api.gamemetadata'),
        ),
        migrations.CreateModel(
            name='Action',
            fields=[
                ('id', models.BigAutoField(auto_created=True, primary_key=True, serialize=False, verbose_name='ID')),
                ('json_blob', models.JSONField(default=dict)),
                ('timestamp', models.DateTimeField(default=django.utils.timezone.now)),
                ('session', models.ForeignKey(on_delete=django.db.models.deletion.CASCADE, to='analytics_api.session')),
                ('task', models.ForeignKey(on_delete=django.db.models.deletion.CASCADE, to='analytics_api.task')),
            ],
        ),
    ]