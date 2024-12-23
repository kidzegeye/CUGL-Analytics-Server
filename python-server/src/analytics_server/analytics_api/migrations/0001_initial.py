# Generated by Django 4.2.16 on 2024-12-13 19:12

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
                ('game_name', models.TextField()),
                ('version_number', models.TextField()),
            ],
            options={
                'db_table': 'analytics_api_game',
            },
        ),
        migrations.CreateModel(
            name='Organization',
            fields=[
                ('id', models.BigAutoField(auto_created=True, primary_key=True, serialize=False, verbose_name='ID')),
                ('organization_name', models.TextField(unique=True)),
            ],
            options={
                'db_table': 'analytics_api_organization',
            },
        ),
        migrations.CreateModel(
            name='Session',
            fields=[
                ('id', models.BigAutoField(auto_created=True, primary_key=True, serialize=False, verbose_name='ID')),
                ('started_at', models.DateTimeField(default=django.utils.timezone.now)),
                ('ended_at', models.DateTimeField(blank=True, null=True)),
                ('ended', models.BooleanField(default=False)),
            ],
            options={
                'db_table': 'analytics_api_session',
            },
        ),
        migrations.CreateModel(
            name='Task',
            fields=[
                ('id', models.BigAutoField(auto_created=True, primary_key=True, serialize=False, verbose_name='ID')),
                ('task_name', models.TextField(unique=True)),
                ('game', models.ForeignKey(on_delete=django.db.models.deletion.CASCADE, to='analytics_api.game')),
            ],
            options={
                'db_table': 'analytics_api_task',
            },
        ),
        migrations.CreateModel(
            name='User',
            fields=[
                ('id', models.BigAutoField(auto_created=True, primary_key=True, serialize=False, verbose_name='ID')),
                ('vendor_id', models.TextField()),
                ('platform', models.TextField()),
                ('game', models.ForeignKey(on_delete=django.db.models.deletion.CASCADE, to='analytics_api.game')),
            ],
            options={
                'db_table': 'analytics_api_user',
            },
        ),
        migrations.CreateModel(
            name='TaskAttempt',
            fields=[
                ('id', models.BigAutoField(auto_created=True, primary_key=True, serialize=False, verbose_name='ID')),
                ('task_attempt_uuid', models.TextField(unique=True)),
                ('started_at', models.DateTimeField(default=django.utils.timezone.now)),
                ('ended_at', models.DateTimeField(blank=True, null=True)),
                ('status', models.TextField(choices=[('not_started', 'Not Started'), ('pending', 'Pending'), ('succeeded', 'Succeeded'), ('failed', 'Failed'), ('preempted', 'Preemped')])),
                ('num_failures', models.IntegerField(default=0)),
                ('statistics', models.JSONField(default=dict)),
                ('session', models.ForeignKey(on_delete=django.db.models.deletion.CASCADE, to='analytics_api.session')),
                ('task', models.ForeignKey(on_delete=django.db.models.deletion.CASCADE, to='analytics_api.task')),
            ],
            options={
                'db_table': 'analytics_api_task_attempt',
            },
        ),
        migrations.AddField(
            model_name='session',
            name='user',
            field=models.ForeignKey(on_delete=django.db.models.deletion.CASCADE, to='analytics_api.user'),
        ),
        migrations.AddField(
            model_name='game',
            name='organization',
            field=models.ForeignKey(on_delete=django.db.models.deletion.CASCADE, to='analytics_api.organization'),
        ),
        migrations.CreateModel(
            name='Action',
            fields=[
                ('id', models.BigAutoField(auto_created=True, primary_key=True, serialize=False, verbose_name='ID')),
                ('json_blob', models.JSONField(default=dict)),
                ('timestamp', models.DateTimeField(default=django.utils.timezone.now)),
                ('session', models.ForeignKey(on_delete=django.db.models.deletion.CASCADE, to='analytics_api.session')),
                ('task_attempts', models.ManyToManyField(blank=True, to='analytics_api.taskattempt')),
            ],
            options={
                'db_table': 'analytics_api_action',
            },
        ),
        migrations.AddConstraint(
            model_name='user',
            constraint=models.UniqueConstraint(fields=('game', 'vendor_id', 'platform'), name='unique user'),
        ),
        migrations.AddConstraint(
            model_name='task',
            constraint=models.UniqueConstraint(fields=('task_name', 'game'), name='unique task'),
        ),
        migrations.AddConstraint(
            model_name='session',
            constraint=models.UniqueConstraint(condition=models.Q(('ended', False)), fields=('user', 'ended'), name='user has only one active session at a time'),
        ),
        migrations.AddConstraint(
            model_name='game',
            constraint=models.UniqueConstraint(fields=('organization', 'game_name', 'version_number'), name='unique game version per organization'),
        ),
    ]
