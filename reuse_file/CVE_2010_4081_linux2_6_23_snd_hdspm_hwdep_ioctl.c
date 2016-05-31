

static int CVE_2010_4081_linux2_6_23_snd_hdspm_hwdep_ioctl(struct snd_hwdep * hw, struct file *file,
				 unsigned int cmd, unsigned long arg)
{
	struct hdspm *hdspm = (struct hdspm *) hw->private_data;
	struct hdspm_mixer_ioctl mixer;
	struct hdspm_config_info info;
	struct hdspm_version hdspm_version;
	struct hdspm_peak_rms_ioctl rms;

	switch (cmd) {

		
	case SNDRV_HDSPM_IOCTL_GET_PEAK_RMS:
		if (copy_from_user(&rms, (void __user *)arg, sizeof(rms)))
			return -EFAULT;
		/* maybe there is a chance to memorymap in future so dont touch just copy */
		if(copy_to_user_fromio((void __user *)rms.peak,
				       hdspm->iobase+HDSPM_MADI_peakrmsbase,
				       sizeof(struct hdspm_peak_rms)) != 0 )
			return -EFAULT;

		break;
		

	case SNDRV_HDSPM_IOCTL_GET_CONFIG_INFO:

		spin_lock_irq(&hdspm->lock);
		info.pref_sync_ref =
		    (unsigned char) hdspm_pref_sync_ref(hdspm);
		info.wordclock_sync_check =
		    (unsigned char) hdspm_wc_sync_check(hdspm);

		info.system_sample_rate = hdspm->system_sample_rate;
		info.autosync_sample_rate =
		    hdspm_external_sample_rate(hdspm);
		info.system_clock_mode =
		    (unsigned char) hdspm_system_clock_mode(hdspm);
		info.clock_source =
		    (unsigned char) hdspm_clock_source(hdspm);
		info.autosync_ref =
		    (unsigned char) hdspm_autosync_ref(hdspm);
		info.line_out = (unsigned char) hdspm_line_out(hdspm);
		info.passthru = 0;
		spin_unlock_irq(&hdspm->lock);
		if (copy_to_user((void __user *) arg, &info, sizeof(info)))
			return -EFAULT;
		break;

	case SNDRV_HDSPM_IOCTL_GET_VERSION:
		hdspm_version.firmware_rev = hdspm->firmware_rev;
		if (copy_to_user((void __user *) arg, &hdspm_version,
				 sizeof(hdspm_version)))
			return -EFAULT;
		break;

	case SNDRV_HDSPM_IOCTL_GET_MIXER:
		if (copy_from_user(&mixer, (void __user *)arg, sizeof(mixer)))
			return -EFAULT;
		if (copy_to_user
		    ((void __user *)mixer.mixer, hdspm->mixer, sizeof(struct hdspm_mixer)))
			return -EFAULT;
		break;

	default:
		return -EINVAL;
	}
	return 0;
}