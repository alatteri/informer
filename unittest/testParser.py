# ------------------------------------------------------------------------------
# START BOOTSTRAP
# ------------------------------------------------------------------------------
try:
    import instinctual
except ImportError, e:
    import os
    import sys
    lib = os.sep.join(os.path.abspath(__file__).split(os.sep)[:-2] + ['lib'])
    sys.path.append(lib)
    import sitecustomize
# ------------------------------------------------------------------------------
# END BOOTSTRAP
# ------------------------------------------------------------------------------

import unittest

from instinctual.informer import parseSetup

class ParseSetupTestCase(unittest.TestCase):
    def assertSetup(self, setup, project=None, shot=None):
        x = parseSetup(setup)
        if project:
            self.assertEquals(project, x['project'])
        if shot:
            self.assertEquals(shot, x['shot'])

    def test1(self):
        self.assertSetup('/hosts/fred/usr/discreet/project/fsnn_57860_mlb/batch/0001-03.batch.', project='fsnn_57860_mlb', shot='0001')

    def test2(self):
        self.assertSetup('/hosts/fred/usr/discreet/project/disc_59190_last/batch/in_every_tribe_v3.batch.', project='disc_59190_last', shot='in_every_tribe')

    def test3(self):
        self.assertSetup('/hosts/fred/usr/discreet/project/disc_59190_last/batch/six_outsidersV2.batch.', project='disc_59190_last', shot='six_outsiders')

    def test4(self):
        self.assertSetup('/hosts/fred/usr/discreet/project/fsnn_57860_mlb/batch/0001-13_Lights2', project='fsnn_57860_mlb', shot='0001')

    def test5(self):
        self.assertSetup('/hosts/fred/usr/discreet/project/fsnn_57860_mlb/batch/0002_net_warp.batch.', project='fsnn_57860_mlb', shot='0002_net_warp')

    def test6(self):
        self.assertSetup('/hosts/fred/usr/discreet/project/fsnn_57860_mlb/batch/0016_new-09b.batch.', project='fsnn_57860_mlb', shot='0016_new')

    def test7(self):
        self.assertSetup('/hosts/fred/usr/discreet/project/fsnn_57860_mlb/batch/0018_snap_zoom_v2.batch.', project='fsnn_57860_mlb', shot='0018_snap_zoom')

    def test8(self):
        self.assertSetup('/hosts/fred/usr/discreet/project/toon_58780_bigmovie_HD/batch/stacked/sc01_v4a.batch.', project='toon_58780_bigmovie_HD', shot='sc01')

    def test9(self):
        self.assertSetup('/hosts/lou/usr/discreet/project/disc_55120_planet/batch/Tags/10HDtv_starting_tonight_at8.batch.', project='disc_55120_planet', shot='10HDtv_starting_tonight_at8')

    def test10(self):
        self.assertSetup('/hosts/ray/usr/discreet/project/disc_58430_deadliest/batch/endframe_trans_v2.batch.', project='disc_58430_deadliest', shot='endframe_trans')

    def test11(self):
        self.assertSetup('/hosts/ray/usr/discreet/project/disc_58430_deadliest/batch/endframe_v6_pass2.batch.', project='disc_58430_deadliest', shot='endframe')

    def test12(self):
        self.assertSetup('/usr/discreet/project/disc_58940_man/batch/pullwide_v3d.batch.', project='disc_58940_man', shot='pullwide')

    def test13(self):
        self.assertSetup('/usr/discreet/project/disc_58940_man/batch/color_correct/cc_water_pour.batch.', project='disc_58940_man', shot='cc_water_pour')

    def test14(self):
        self.assertSetup('/usr/discreet/project/disc_59190_last/batch/champion_v3.batch.', project='disc_59190_last', shot='champion')

    def test15(self):
        self.assertSetup('/usr/discreet/project/disc_59190_last/batch/fight_of_livesV1.batch.', project='disc_59190_last', shot='fight_of_lives')

    def test16(self):
        self.assertSetup('/usr/discreet/project/disc_59190_last/batch/titletodisclogoUpdatev1.batch.', project='disc_59190_last', shot='titletodisclogoUpdate')

    def test17(self):
        self.assertSetup('/usr/discreet/project/disc_59190_last/batch/on_every_continent_v1_.batch.', project='disc_59190_last', shot='on_every_continent')

    def test18(self):
        self.assertSetup('/usr/discreet/project/fnlv_59180_summer/batch/Section_B.batch.', project='fnlv_59180_summer', shot='Section_B')

    def test19(self):
        self.assertSetup('/usr/discreet/project/fsnn_57860_mlb/batch/0001-10.batch.', project='fsnn_57860_mlb', shot='0001')

    def test20(self):
        self.assertSetup('/usr/discreet/project/fsnn_57860_mlb/batch/0003_drays.batch.', project='fsnn_57860_mlb', shot='0003_drays')

    def test21(self):
        self.assertSetup('/usr/discreet/project/fsnn_57860_mlb/batch/0004-02.batch.', project='fsnn_57860_mlb', shot='0004')

    def test22(self):
        self.assertSetup('/usr/discreet/project/fsnn_57860_mlb/batch/0006-05_elmnts_for_avid.batch.', project='fsnn_57860_mlb', shot='0006')

    def test23(self):
        self.assertSetup('/usr/discreet/project/fsnn_57860_mlb/batch/0010-03_matte_output.batch.', project='fsnn_57860_mlb', shot='0010')

    def test24(self):
        self.assertSetup('/usr/discreet/project/hbon_58910_spring/batch/DVD_title05_v1.batch.', project='hbon_58910_spring', shot='DVD_title05')

    def test25(self):
        self.assertSetup('/usr/discreet/project/ngeo_57900_upfront/batch/sample_photo2_part6_v2.batch.', project='ngeo_57900_upfront', shot='sample_photo2_part6')

    def test26(self):
        self.assertSetup('/usr/discreet/project/ngeo_57900_upfront/batch/SeriesTrans05c_textless.batch.', project='ngeo_57900_upfront', shot='SeriesTrans05c_textless')

    def test27(self):
        self.assertSetup('/usr/discreet/project/patr_58630_allaccess/batch/board2_v2.batch.', project='patr_58630_allaccess', shot='board2')

    def test28(self):
        self.assertSetup('/usr/discreet/project/toon_58780_bigmovie_HD/batch/OpenProduced_Scene_09_to_10_v4.batch.', project='toon_58780_bigmovie_HD', shot='OpenProduced_Scene_09_to_10')

    def test29(self):
        self.assertSetup('/usr/discreet/project/trav_59050_ad/batch/nature_cc/nature_v6_shot01_cc_v2.batch.', project='trav_59050_ad', shot='nature_v6_shot01_cc')

    def test30(self):
        self.assertSetup('/usr/discreet/project/trav_59050_ad/batch/nature_cc/nature_v6_shot01_cc.batch.', project='trav_59050_ad', shot='nature_v6_shot01_cc')

    def test31(self):
        self.assertSetup('/usr/discreet/project/trav_59050_ad/batch/Shot_06.batch.', project='trav_59050_ad', shot='Shot_06')

    def test32(self):
        self.assertSetup('/usr/discreet/project/vers_59100_college/batch/VS_30_v4_ExtraJuice.batch.', project='vers_59100_college', shot='VS_30_v4_ExtraJuice')

    def test33(self):
        self.assertSetup('/hosts/fugu/usr/discreet/project/COKHF2_1457P_fl2007_fugu/batch/AFRICA_90VER_VendingMachine_3_b2.batch.', project='COKHF2_1457P_fl2007_fugu', shot='AFRICA_90VER_VendingMachine_3')

    def test34(self):
        self.assertSetup('/hosts/servo/usr/discreet/project/Martel_1529P_fl2007_servo/batch/pda_shot_b5.batch.', project='Martel_1529P_fl2007_servo', shot='pda_shot')

    def test35(self):
        self.assertSetup('/usr/discreet/project/Martel_1529P_fl2007_fresh/batch/pda_shot_b10.batch.', project='Martel_1529P_fl2007_fresh', shot='pda_shot')

    def test36(self):
        self.assertSetup('/usr/discreet/project/MYSIMS_1532B_fl2007_Fresh/batch/SC_012_cleanUp_b2.batch.', project='MYSIMS_1532B_fl2007_Fresh', shot='SC_012_cleanUp')

    #def testXXX(self):
    #    self.assertSetup('', project='', shot='')

if __name__ == '__main__':
    unittest.main()
