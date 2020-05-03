import os
import numpy as np
import cv2

class Config:
    RANGE_MAX = 300
    INTENSITY_MAX = 4095
    X_DIM = 0
    Y_DIM = 1
    Z_DIM = 2
    INTENSITY_DIM = 3
    POINT_IDX_DIM = 4
    LAST_RETURN_IDX_DIM = 5
    LABEL_DIM = 6
    NUM_DIMS = 7
    NUM_SCAN_LINES = 64
    NUM_POINTS_PER_LINE = 1000
    NUM_POINTS = NUM_SCAN_LINES * NUM_POINTS_PER_LINE
    MAX_BLOOM_OFFSET = 50

def folder_iterator(folder_name):
    """
    Iterate through all the .xyz files in the folder
    """
    for xyz_file in sorted(os.listdir(folder_name)):
        if xyz_file[-4:] == ".xyz":
            try:
                pcl = np.loadtxt(os.path.join(folder_name, xyz_file), delimiter=',')
                if pcl.shape[0] != Config.NUM_POINTS or pcl.shape[1] != Config.NUM_DIMS:
                    print(xyz_file, " Dimension mismatch.")
                front_projection = pcl.reshape(Config.NUM_SCAN_LINES, Config.NUM_POINTS_PER_LINE, Config.NUM_DIMS)
                yield front_projection
            except:
                print(xyz_file, " has an issue.")

def main():
    for front_projection in folder_iterator('.'):
        xs = np.clip(front_projection[:, :, Config.X_DIM] / 50, 0, 1)
        intensities = np.clip(front_projection[:, :, Config.INTENSITY_DIM] / 500, 0, 1)
        labels = front_projection[:, :, Config.LABEL_DIM].astype(np.float32)

        cv2.namedWindow('frame')
        cv2.moveWindow('labels', 10, 10)
        cv2.imshow('frame', np.vstack((xs, intensities, labels)))
        cv2.waitKey(0)

if __name__ == '__main__':
    main()
